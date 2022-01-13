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
    actions->pushMovement(QPoint(x, y));
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
