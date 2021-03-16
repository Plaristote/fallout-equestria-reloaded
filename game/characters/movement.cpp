#include "movement.h"
#include "game.h"
#include <QJsonArray>
#include <QJsonObject>

CharacterMovement::CharacterMovement(QObject *parent) : CHARACTER_BASE_OBJECT(parent)
{
  orientation  = "down";
  movementMode = "walking";
  connect(this, &Sprite::movementFinished,              this, &CharacterMovement::onMovementEnded);
  connect(this, &CharacterMovement::reachedDestination, this, &CharacterMovement::onDestinationReached);
}

void CharacterMovement::setAnimation(const QString &animationName)
{
  bool orientedAnimation = animationName != "death";

  Sprite::setAnimation(orientedAnimation
                       ? animationName + '-' + orientation
                       : animationName);
}

void CharacterMovement::lookTo(int x, int y)
{
  if (position.x() > x && position.y() > y)
    orientation = "up";
  else if (position.x() < x && position.y() < y)
    orientation = "down";
  else if (position.x() == x && position.y() > y)
    orientation = "right";
  else if (position.x() == x && position.y() < y)
    orientation = "left";
  else if (position.x() >= x)
    orientation = "left";
  else
    orientation = "right";
}

void CharacterMovement::moveTo(int x, int y)
{
  auto* level = Game::get()->getLevel();

  if (level)
  {
    QString animationName;
    QPoint  renderPosition = level->getRenderPositionForTile(x, y);

    lookTo(x, y);
    animationName = movementMode + '-' + orientation;
    moveToCoordinates(renderPosition);
    if (getCurrentAnimation() != animationName)
      setAnimation(movementMode);
    level->getGrid()->moveObject(this, x, y);
    level->getGrid()->triggerZone(this, x, y);
  }
}

void CharacterMovement::setMovementMode(const QString& mode)
{
  movementMode  = mode;
  movementSpeed = mode == "walking" ? 100 : 200;
}

void CharacterMovement::onIdle()
{
  setAnimation("idle");
}

void CharacterMovement::onMovementEnded()
{
  if (script)
    script->call("onMovementEnded");
}

void CharacterMovement::onDestinationReached()
{
  if (script)
    script->call("onDestinationReached");
}

void CharacterMovement::load(const QJsonObject& data)
{
  const QJsonArray jsonPath = data["currentPath"].toArray();

  for (const QJsonValue& pathPointData : jsonPath)
  {
    QPoint pathPoint;

    pathPoint.setX(pathPointData["x"].toInt());
    pathPoint.setY(pathPointData["y"].toInt());
    currentPath << pathPoint;
  }
  orientation = data["orientation"].toString("down");
  if (orientation.length() == 0)
    orientation = "down";
  setMovementMode(data["moveMode"].toString("walking"));
  CHARACTER_BASE_OBJECT::load(data);
}

void CharacterMovement::save(QJsonObject& data) const
{
  QJsonArray currentPathData;

  for (QPoint pathPoint : currentPath)
  {
    QJsonObject pathPointData;

    pathPointData["x"] = pathPoint.x();
    pathPointData["y"] = pathPoint.y();
    currentPathData << pathPointData;
  }
  data["currentPath"] = currentPathData;
  data["orientation"] = orientation;
  data["moveMode"]    = movementMode;
  CHARACTER_BASE_OBJECT::save(data);
}
