#include "spriteanimationpart.h"
#include <QJSValue>
#include <QDebug>
#include "game/leveltask.h"
#include "game/objects/components/sprite.h"
#include <cmath>

SpriteAnimationPart::~SpriteAnimationPart()
{
  auto* currentLevel = LevelTask::get();

  if (currentLevel == level)
    level->unregisterVisualEffect(sprite);
  delete sprite;
}

bool SpriteAnimationPart::matches(const QJSValue& descriptor)
{
  return descriptor.property("type").toString() == "Sprite";
}

void SpriteAnimationPart::initialize(QJSValue &value)
{
  sprite = new Sprite();
  level  = LevelTask::get();
  sprite->setProperty("floating", true);
  sprite->setSpriteName(value.property("name").toString());
  sprite->setAnimation(value.property("animation").toString());
  if (value.hasProperty("fromX") && value.hasProperty("fromY")) {
    from = QPoint(
      value.property("fromX").toInt(),
      value.property("fromY").toInt()
    );
  }
  else
    qDebug() << "SpriteAnimationPart: missing fromX and/or fromY properties";
  if (value.hasProperty("toX") && value.hasProperty("toY")) {
    to = QPoint(
      value.property("toX").toInt(),
      value.property("toY").toInt()
    );
  }
  else
    to = from;
  if (from != to)
    initializeMovement(value);
  else
    QObject::connect(sprite, &Sprite::animationFinished, std::bind(&SpriteAnimationPart::onAnimationFinished, this));
}

void SpriteAnimationPart::initializeMovement(QJSValue& settings)
{
  if (settings.hasProperty("speed"))
    sprite->setMovementSpeed(static_cast<float>(settings.property("speed").toNumber()));
  // A position callback exists and will be used to compute movement
  if (settings.hasProperty("position") && settings.property("position").isCallable())
  {
    position = settings.property("position");
    stepCount  = std::sqrt(std::pow(from.x() - to.x(), 2) + std::pow(from.y() - to.y(), 2)) * 10;
    stepCount /= sprite->getMovementSpeed();
    timer.setInterval(animationInterval);
    timer.setSingleShot(false);
    QObject::connect(&timer, &QTimer::timeout, std::bind(&SpriteAnimationPart::onRefreshPosition, this));
  }
  // No position callback provided, so we'll use the generic sprite movement system
  else
  {
    sprite->moveToCoordinates(to);
    QObject::connect(sprite, &Sprite::movementFinished, std::bind(&SpriteAnimationPart::onAnimationFinished, this));
  }
}

void SpriteAnimationPart::onRefreshPosition()
{
  float percentage = static_cast<float>(stepIt) / static_cast<float>(stepCount);
  QJSValue result = position.call(QJSValueList() << percentage);

  sprite->setRenderPosition(QPoint{
    result.property("x").toInt(),
    result.property("y").toInt()
  });
  if (++stepIt >= stepCount)
  {
    onAnimationFinished();
    timer.stop();
  }
}

void SpriteAnimationPart::start()
{
  level->registerVisualEffect(sprite);
  sprite->setRenderPosition(from);
  qDebug() << "SpriteAnimationPart::start at" << from;
  if (from != to)
  {
    if (position.isCallable())
      timer.start();
    else
      sprite->moveToCoordinates(to);
  }
}
