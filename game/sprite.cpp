#include "sprite.h"
#include <cmath>
#include <QDebug>

Sprite::Sprite(QObject *parent) : QObject(parent)
{
  movementSpeed = 10;
  animationTimer.setSingleShot(true);
  movementTimer.setSingleShot(true);
  movementTimer.setInterval(100);
  connect(&animationTimer, &QTimer::timeout, this, &Sprite::runAnimation);
  connect(&movementTimer,  &QTimer::timeout, this, &Sprite::runMovement);
}

void Sprite::setAnimation(const QString &animationName)
{
  auto* library = AnimationLibrary::get();

  animationTimer.stop();
  animation = library->getAnimation(name, animationName);
  if (animation.frameCount > 1)
    animationTimer.start(animation.frameInterval);
}

void Sprite::runAnimation()
{
  auto width = animation.clippedRect.width();

  animation.currentFrame++;
  if (animation.currentFrame >= animation.frameCount)
  {
    animation.currentFrame = 0;
    animation.clippedRect.setX(animation.firstFramePosition.x());
    animation.clippedRect.setWidth(width);
    if (animation.repeat)
      animationTimer.start(animation.frameInterval);
    else
      emit animationFinished();
  }
  else
  {
    animation.clippedRect.adjust(width, 0, width, 0);
    animationTimer.start(animation.frameInterval);
  }
}

void Sprite::forceMoveToCoordinates(QPoint coordinates)
{
  movementTimer.stop();
  spritePosition = coordinates;
  spriteMovementTarget = coordinates;
}

void Sprite::moveToCoordinates(QPoint coordinates)
{
  spriteMovementTarget = coordinates;
  if (!movementTimer.isActive())
    movementTimer.start();
}

static int axisMovement(int current, int final, int speed)
{
  int result = final;

  if      (current < final && final - current > speed) { result = current + speed; }
  else if (current > final && current - final > speed) { result = current - speed; }
  return std::abs(result - final) < speed ? final : result;
}

void Sprite::runMovement()
{
  float movementSpeedX = movementSpeed;
  float movementSpeedY = movementSpeed;
  int   distX = std::max(spritePosition.x(), spriteMovementTarget.x()) - std::min(spritePosition.x(), spriteMovementTarget.x());
  int   distY = std::max(spritePosition.y(), spriteMovementTarget.y()) - std::min(spritePosition.y(), spriteMovementTarget.y());

  if      (distX > distY) { movementSpeedY = movementSpeed * (static_cast<float>(distY) / static_cast<float>(distX)); }
  else if (distY > distX) { movementSpeedX = movementSpeed * (static_cast<float>(distX) / static_cast<float>(distY)); }
  spritePosition.setX(axisMovement(spritePosition.x(), spriteMovementTarget.x(), static_cast<int>(movementSpeedX)));
  spritePosition.setY(axisMovement(spritePosition.y(), spriteMovementTarget.y(), static_cast<int>(movementSpeedY)));
  if (spritePosition != spriteMovementTarget)
    movementTimer.start();
  else
  {
    emit movementFinished(this);
  }
}
