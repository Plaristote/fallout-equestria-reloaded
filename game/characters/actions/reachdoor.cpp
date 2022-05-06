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
      QList<Point> path = getPath();

      character->rcurrentPath() = path;
      if (path.size())
        state = canMakeNextMovement() ? InProgress : Interrupted;
      else
        state = Interrupted;
    }
    return state == Done || state == InProgress;
  }
  return ReachAction::trigger();
}

QList<Point> ReachDoorAction::getPath() const
{
  auto& grid = Game::get()->getLevel()->getPathfinder();
  QList<Point> shortestPath;

  for (Point candidate : candidates)
  {
    QList<Point> path;

    grid.findPath(character->getPoint(), candidate, path, character);
    if (path.size() > 0 && (shortestPath.size() == 0 || path.size() < shortestPath.size()))
      shortestPath = path;
  }
  return shortestPath;
}

int ReachDoorAction::getApCost() const
{
  if (range == 0.f)
  {
    auto path = getPath();

    if (path.size() > 0)
      return pathApCost(path);
    return -1;
  }
  return ReachAction::getApCost();
}

bool ReachDoorAction::alreadyReached() const
{
  if (range == 0.f)
    return candidates.contains(character->getPoint());
  return ReachAction::alreadyReached();
}
