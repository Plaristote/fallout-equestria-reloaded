#include "sprite.h"
#include <cmath>
#include <QDebug>

Sprite::Sprite(QObject *parent) : ParentType(parent)
{
  movementSpeed = 100;
  connect(this, &Sprite::spriteChanged, this, &Sprite::spriteSourceChanged);
  connect(this, &Sprite::spriteChanged, this, &Sprite::clippedRectChanged);
}

bool Sprite::isAnimated() const
{
  return animation.currentFrame + 1 <= animation.frameCount;
}

void Sprite::update(qint64 delta)
{
  if (animation.repeat || isAnimated())
  {
    animationElapsedTime += delta;
    while (animationElapsedTime > animation.frameInterval)
      runAnimation();
  }
  if (spritePosition != spriteMovementTarget)
    runMovement(delta);
}

void Sprite::setAnimation(const QString &animationName)
{
  auto* library = AnimationLibrary::get();

  animationElapsedTime = 0;
  animation = library->getAnimation(name, animationName);
  shadow = library->getAnimation(name, "shadow");
  emit spriteSourceChanged();
  emit clippedRectChanged();
}

bool Sprite::hasAnimation(const QString& animationName) const
{
  return AnimationLibrary::get()->hasAnimation(name, animationName);
}

void Sprite::runAnimation()
{
  auto width = animation.clippedRect.width();

  animationElapsedTime = -animation.frameInterval;
  animation.currentFrame++;
  if (animation.currentFrame >= animation.frameCount)
  {
    if (animation.repeat)
    {
      animation.currentFrame = 0;
      animation.clippedRect.setX(animation.firstFramePosition.x());
      animation.clippedRect.setWidth(width);
    }
    else
    {
      animationElapsedTime = 0;
      emit animationFinished();
    }
  }
  else
  {
    auto movement = width * (animation.reverse ? -1 : 1);

    animation.clippedRect.adjust(movement, 0, movement, 0);
  }
  emit clippedRectChanged();
}

const QImage& Sprite::getImage() const
{
  const QImage& image = AnimationLibrary::get()->getImage(animation.source);

  return image;
}


void Sprite::setRenderPosition(QPoint coordinates)
{
  spritePosition = coordinates;
  spriteMovementTarget = coordinates;
  emit spritePositionChanged();
}

void Sprite::moveToCoordinates(QPoint coordinates)
{
  spriteMovementTarget = coordinates;
}

static int axisMovement(int current, int final, int speed)
{
  int result = final;

  if      (current < final && final - current > speed) { result = current + speed; }
  else if (current > final && current - final > speed) { result = current - speed; }
  return std::abs(result - final) < speed ? final : result;
}

void Sprite::runMovement(qint64 delta)
{
  float movementSpeed  = this->movementSpeed * static_cast<float>(delta) / 1000.f;
  float movementSpeedX = movementSpeed;
  float movementSpeedY = movementSpeed;
  int   distX = std::max(spritePosition.x(), spriteMovementTarget.x()) - std::min(spritePosition.x(), spriteMovementTarget.x());
  int   distY = std::max(spritePosition.y(), spriteMovementTarget.y()) - std::min(spritePosition.y(), spriteMovementTarget.y());

  if      (distX > distY) { movementSpeedY = movementSpeed * (static_cast<float>(distY) / static_cast<float>(distX)); }
  else if (distY > distX) { movementSpeedX = movementSpeed * (static_cast<float>(distX) / static_cast<float>(distY)); }
  spritePosition.setX(axisMovement(spritePosition.x(), spriteMovementTarget.x(), static_cast<int>(movementSpeedX)));
  spritePosition.setY(axisMovement(spritePosition.y(), spriteMovementTarget.y(), static_cast<int>(movementSpeedY)));
  emit spritePositionChanged();
  if (spritePosition == spriteMovementTarget)
    emit movementFinished(this);
}

void Sprite::load(const QJsonObject& data)
{
  setSpriteName(data["spriteName"].toString());
  spritePosition.setX(data["rx"].toInt()); spritePosition.setY(data["ry"].toInt());
  spriteMovementTarget.setX(data["mtx"].toInt()); spriteMovementTarget.setY(data["mty"].toInt());
  floating = data["float"].toBool();
  Sprite::setAnimation(data["animation"].toString());
  ParentType::load(data);
}

void Sprite::save(QJsonObject& data) const
{
  data["spriteName"] = name;
  data["rx"]  = spritePosition.x();       data["ry"] = spritePosition.y();
  data["mtx"] = spriteMovementTarget.x(); data["mty"] = spriteMovementTarget.y();
  data["animation"] = animation.name;
  data["float"] = floating;
  ParentType::save(data);
}
