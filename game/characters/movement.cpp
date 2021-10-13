#include "movement.h"
#include "game.h"
#include <QJsonArray>
#include <QJsonObject>

CharacterMovement::CharacterMovement(QObject *parent) : ParentType(parent)
{
  setOrientation(BottomDir);
  movementMode = "walking";
  connect(this, &Sprite::movementFinished,              this, &CharacterMovement::onMovementEnded);
  connect(this, &CharacterMovement::reachedDestination, this, &CharacterMovement::onDestinationReached);
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

void CharacterMovement::moveTo(Point target)
{
  auto* level      = Game::get()->getLevel();
  auto* originGrid = level ? level->getFloorGrid(getCurrentFloor()) : nullptr;
  auto* targetGrid = level ? level->getFloorGrid(target.z) : nullptr;

  if (originGrid && targetGrid)
  {
    QString animationName;
    QPoint  renderPosition = level->getRenderPositionForTile(target.x, target.y, target.z);

    renderPosition.ry() -= targetGrid->getTilemap()->getTileSize().height() / 4;
    lookTo(target.x, target.y);
    animationName = movementMode + '-' + getOrientationName();
    moveToCoordinates(renderPosition);
    if (getCurrentAnimation() != animationName)
      setAnimation(movementMode);
    originGrid->extractObject(this);
    targetGrid->insertObject(this, target.x, target.y);
    targetGrid->triggerZone(this, target.x, target.y);
    onMovementStart();
  }
}

void CharacterMovement::setMovementMode(const QString& mode)
{
  movementMode  = mode;
  movementSpeed = mode == "walking" ? 125 : 260;
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
    Point pathPoint;

    pathPoint.x = pathPointData["x"].toInt();
    pathPoint.y = pathPointData["y"].toInt();
    pathPoint.z = pathPointData["z"].toInt();
    currentPath << pathPoint;
  }
  setMovementMode(data["moveMode"].toString("walking"));
  ParentType::load(data);
}

void CharacterMovement::save(QJsonObject& data) const
{
  QJsonArray currentPathData;

  for (Point pathPoint : currentPath)
  {
    QJsonObject pathPointData;

    pathPointData["x"] = pathPoint.x;
    pathPointData["y"] = pathPoint.y;
    pathPointData["z"] = pathPoint.z;
    currentPathData << pathPointData;
  }
  data["currentPath"] = currentPathData;
  data["moveMode"]    = movementMode;
  ParentType::save(data);
}
