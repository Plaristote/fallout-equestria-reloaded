#include "reach.h"
#include "game.h"
#include <cmath>

QVector<QPoint> ReachAction::getCandidates(int caseDistance)
{
  QVector<QPoint> candidates;
  QPoint position = object->getPosition();

  candidates.reserve(caseDistance * caseDistance);
  for (int x = position.x() - caseDistance ; x < position.x() + caseDistance ; ++x)
  {
    for (int y = position.y() - caseDistance ; y < position.y() + caseDistance ; ++y)
    {
      if (x != position.x() || y != position.y())
        candidates << QPoint(x, y);
    }
  }
  std::sort(candidates.begin(), candidates.end(), [this](QPoint a, QPoint b)
  {
    return character->getDistance(a) < character->getDistance(b);
  });
  return candidates;
}

bool ReachAction::trigger()
{
  if (range == 0.f)
  {
    target = object->getPosition();
    return MovementAction::trigger();
  }
  else if (character->getDistance(object) <= range && character->hasLineOfSight(object))
    state = Done;
  else
  {
    auto* level = Game::get()->getLevel();
    auto* grid  = level->getGrid();
    int caseDistance = static_cast<int>(std::floor(range));
    auto candidates = getCandidates(caseDistance);

    state = Interrupted;
    for (auto it = candidates.begin() ; it != candidates.end() ; ++it)
    {
      if (grid->findPath(character->getPosition(), *it, character->rcurrentPath()))
      {
        state = InProgress;
        break ;
      }
    }
  }
  return state == Done || state == InProgress;
}

void ReachAction::triggerNextMovement()
{
  MovementAction::triggerNextMovement();
  if (state == Interrupted)
    trigger();
}
