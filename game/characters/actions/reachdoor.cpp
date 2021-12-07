#include "reachdoor.h"
#include "game.h"

bool ReachDoorAction::trigger()
{
  if (range == 0.f)
  {
    if (alreadyReached())
      state = Done;
    else
    {
      auto& grid = Game::get()->getLevel()->getPathfinder();
      auto candidates = doorway->getInteractionPositions();

      state = Interrupted;
      if (grid.findPath(character->getPoint(), candidates, character->rcurrentPath(), character, true))
        state = canMakeNextMovement() ? InProgress : Interrupted;
    }
    return state == Done || state == InProgress;
  }
  return ReachAction::trigger();
}

int ReachDoorAction::getApCost() const
{
  if (range == 0.f)
    return getApCostForCandidates(doorway->getInteractionPositions(), true);
  return ReachAction::getApCost();
}

bool ReachDoorAction::alreadyReached() const
{
  if (range == 0.f)
    return doorway->getInteractionPositions().contains(character->getPoint());
  return ReachAction::alreadyReached();
}
