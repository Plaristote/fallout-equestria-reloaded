#include "sprite.h"
#include <cmath>
#include <QDebug>

Sprite::Sprite(QObject *parent) : QObject(parent)
{
  animationTimer.setSingleShot(true);
  movementTimer.setSingleShot(true);
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

void Sprite::runMovement()
{
  QPoint delta = spriteMovementTarget - spritePosition;
  float angle = std::atan2<float>(static_cast<float>(delta.x()), static_cast<float>(delta.y()));

  spritePosition.rx() += static_cast<int>(movementSpeed * std::cos(angle));
  spritePosition.ry() += static_cast<int>(movementSpeed * std::sin(angle));
  if (std::abs(spritePosition.x() - spriteMovementTarget.x()) < static_cast<int>(movementSpeed))
    spritePosition.setX(spriteMovementTarget.x());
  if (std::abs(spritePosition.y() - spriteMovementTarget.y()) < static_cast<int>(movementSpeed))
    spritePosition.setY(spriteMovementTarget.y());
  if (spritePosition != spriteMovementTarget)
    movementTimer.start();
  else
    emit movementFinished();
}
