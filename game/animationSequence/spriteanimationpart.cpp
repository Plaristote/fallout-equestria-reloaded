#include "spriteanimationpart.h"
#include <QJSValue>
#include <QDebug>
#include "game.h"
#include "game/leveltask.h"
#include "game/objects/components/sprite.h"

SpriteAnimationPart::~SpriteAnimationPart()
{
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
  level  = Game::get()->getLevel();
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
  {
    if (value.hasProperty("speed"))
      sprite->setMovementSpeed(static_cast<float>(value.property("speed").toNumber()));
    sprite->moveToCoordinates(to);
    QObject::connect(sprite, &Sprite::movementFinished, std::bind(&SpriteAnimationPart::onAnimationFinished, this));
  }
  else
    QObject::connect(sprite, &Sprite::animationFinished, std::bind(&SpriteAnimationPart::onAnimationFinished, this));
}

void SpriteAnimationPart::start()
{
  level->registerVisualEffect(sprite);
  sprite->setRenderPosition(from);
  qDebug() << "SpriteAnimationPart::start at" << from;
  if (from != to)
    sprite->moveToCoordinates(to);
}
