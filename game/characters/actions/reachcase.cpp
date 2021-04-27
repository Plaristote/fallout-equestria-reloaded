#include "reachcase.h"

bool ReachCaseAction::alreadyReached() const
{
  return character->getDistance(target) <= range && character->hasLineOfSight(target);
}
