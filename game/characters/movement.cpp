#include "movement.h"
#include "game.h"
#include <QJsonArray>
#include <QJsonObject>

CharacterMovement::CharacterMovement(QObject *parent) : ParentType(parent)
{
  orientation  = BottomDir;
  movementMode = "walking";
  connect(this, &Sprite::movementFinished,              this, &CharacterMovement::onMovementEnded);
  connect(this, &CharacterMovement::reachedDestination, this, &CharacterMovement::onDestinationReached);
}

void CharacterMovement::setAnimation(const QString &animationName)
{
  ParentType::setAnimation(animationName);
  // Fallback to idle for unexisting character animations
  if (getAnimationBaseName() != animationName && animationName != "idle")
    ParentType::setAnimation("idle");
}

void CharacterMovement::lookTo(int x, int y)
{
  if (position.x() > x && position.y() > y)
    setOrientation(UpperDir);
  else if (position.x() < x && position.y() < y)
    setOrientation(BottomDir);
  else if (position.x() == x && position.y() > y)
    setOrientation(RightDir);
  else if (position.x() == x && position.y() < y)
    setOrientation(LeftDir);
  else if (position.x() >= x)
    setOrientation(LeftDir);
  else
    setOrientation(RightDir);
}

void CharacterMovement::moveTo(int x, int y)
{
  auto* level = Game::get()->getLevel();

  if (level)
  {
    QString animationName;
    QPoint  renderPosition = level->getRenderPositionForTile(x, y);

    renderPosition.ry() -= level->getTileMap()->getTileSize().height() / 4;
    lookTo(x, y);
    animationName = movementMode + '-' + getOrientationName();
    moveToCoordinates(renderPosition);
    if (getCurrentAnimation() != animationName)
      setAnimation(movementMode);
    level->getGrid()->moveObject(this, x, y);
    level->getGrid()->triggerZone(this, x, y);
    onMovementStart();
  }
}

void CharacterMovement::setMovementMode(const QString& mode)
{
  movementMode  = mode;
  movementSpeed = mode == "walking" ? 100 : 170;
}

void CharacterMovement::onIdle()
{
  setAnimation("idle");
}

void CharacterMovement::onMovementStart()
{
  if (script)
    script->call("onMovementStart");
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
  setMovementMode(data["moveMode"].toString("walking"));
  ParentType::load(data);
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
  data["moveMode"]    = movementMode;
  ParentType::save(data);
}
