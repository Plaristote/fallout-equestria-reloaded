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

QString CharacterMovement::getAnimationBaseName() const
{
  static const QRegularExpression regexp("-(up|left|down|right)$");

  return getAnimation().replace(regexp, "");
}

void CharacterMovement::setAnimation(const QString &animationName)
{
  QString completeAnimationName = animationName + '-' + orientation;

  Sprite::setAnimation(completeAnimationName);
  // Fallback to idle for unexisting character animations
  if (getAnimation() != completeAnimationName && animationName != "idle")
    setAnimation("idle");
}

void CharacterMovement::setOrientation(const QString& value)
{
  if (value != orientation)
  {
    orientation = value;
    setAnimation(getAnimationBaseName());
    emit orientationChanged();
  }
}

void CharacterMovement::lookTo(int x, int y)
{
  if (position.x() > x && position.y() > y)
    setOrientation("up");
  else if (position.x() < x && position.y() < y)
    setOrientation("down");
  else if (position.x() == x && position.y() > y)
    setOrientation("right");
  else if (position.x() == x && position.y() < y)
    setOrientation("left");
  else if (position.x() >= x)
    setOrientation("left");
  else
    setOrientation("right");
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
    animationName = movementMode + '-' + orientation;
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
