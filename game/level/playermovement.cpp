#include "playermovement.h"
#include "game.h"
#include "game/characters/actionqueue.h"
#include "i18n.h"

int PlayerMovementComponent::movementModeOption = PlayerMovementComponent::MixedMovementMode;

PlayerMovementComponent::PlayerMovementComponent(QObject *parent) : ParentType(parent)
{
}

void PlayerMovementComponent::setDefaultMovementMode()
{
  switch (movementModeOption)
  {
    case RunMovementMode:
      getPlayer()->setMovementMode("running");
      break ;
    case WalkMovementMode:
      getPlayer()->setMovementMode("walking");
      break ;
  }
}

void PlayerMovementComponent::movePlayerTo(int x, int y)
{
  DynamicObject* occupant = getFloorGrid(getPlayer()->getCurrentFloor())->getOccupant(x, y);
  auto* actions = getPlayer()->getActionQueue();
  QPoint oldTarget(-1, -1);

  if (getPlayer()->getCurrentPath().length() > 0)
    oldTarget = getPlayer()->getCurrentPath().last();
  actions->reset();
  if (occupant)
    actions->pushReach(occupant, 1);
  else
    actions->pushMovement(Point{x, y, static_cast<unsigned char>(getCurrentFloor())});
  if (!(actions->start()))
    Game::get()->appendToConsole(I18n::get()->t("no-path"));
  else if (!getPlayer()->getCurrentPath().empty())
  {
    switch (movementModeOption)
    {
      case MixedMovementMode:
        getPlayer()->setMovementMode(oldTarget == getPlayer()->getCurrentPath().last() ? "running" : "walking");
        break ;
      default:
        setDefaultMovementMode();
        break ;
    }
    emit playerMovingTo(getPlayer()->getCurrentPath().last());
  }
}

QJSValue PlayerMovementComponent::previewPathTo(int x, int y)
{
  auto& grid = getPathfinder();
  QList<Point> path;
  Point target{x, y, static_cast<unsigned char>(getCurrentFloor())};
  Point lastPosition = getPlayer()->getPoint();
  auto& scriptEngine = *qmlJsEngine;
  QJSValue positions = scriptEngine.newArray();
  QJSValue costs     = scriptEngine.newArray();
  QJSValue result    = scriptEngine.newObject();
  QJSValue push      = positions.property("push");
  auto* player = getPlayer();
  int actionPoints = player ? player->getActionPoints() : 0;
  int totalCost = 0;

  if (actionPoints > 0 && grid.findPath(getPlayer()->getPoint(), target, path, player))
  {
    for (const auto& point : std::as_const(path))
    {
      int cost = grid.actionPointCost(lastPosition, point);
      QJSValue position = scriptEngine.newObject();

      totalCost += cost;
      if (totalCost > actionPoints)
        break ;
      position.setProperty("x", point.x);
      position.setProperty("y", point.y);
      push.callWithInstance(positions, QJSValueList() << position);
      push.callWithInstance(costs, QJSValueList() << totalCost);
      lastPosition = point;
    }
  }
  result.setProperty("positions", positions);
  result.setProperty("costs", costs);
  return result;
}
