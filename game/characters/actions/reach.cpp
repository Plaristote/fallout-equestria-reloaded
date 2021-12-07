#include "reach.h"
#include "game.h"
#include <cmath>

static int rateCase(Game* game, QJSValue callback, const Point position)
{
  return game->scriptCall(callback, QJSValueList() << position.x << position.y << static_cast<int>(position.z), "ReachAction").toInt();
}

static void filterCases(Game* game, QJSValue callback, QVector<Point>& candidates)
{
  for (auto it = candidates.begin() ; it != candidates.end() ;)
  {
    if (rateCase(game, callback, *it) < 0)
      it = candidates.erase(it);
    else
      it++;
  }
}

static bool candidateCompare(Character* character, QPoint a, QPoint b)
{
  return character->getDistance(a) < character->getDistance(b);
}

static bool scriptedCandidateCompare(Character* character, QJSValue callback, Point a, Point b)
{
  Game* game = Game::get();
  int   rateA = rateCase(game, callback, a);
  int   rateB = rateCase(game, callback, b);

  if (rateA < rateB)
    return true;
  return candidateCompare(character, a, b);
}

QVector<Point> ReachAction::getCandidates(int caseDistance) const
{
  Point position = getTargetPosition();
  QVector<Point> candidates;
  Game* game = Game::get();
  std::function<bool (Point, Point)> compare;

  candidates.reserve(caseDistance * caseDistance);
  for (int x = position.x - caseDistance ; x <= position.x + caseDistance ; ++x)
  {
    for (int y = position.y - caseDistance ; y <= position.y + caseDistance ; ++y)
    {
      Point candidatePosition{x, y, position.z};
      LevelGrid* grid = game->getLevel()->getFloorGrid(candidatePosition.z);

      if (candidatePosition != position && !grid->isOccupied(candidatePosition.x, candidatePosition.y) && character->hasSightFrom(position, candidatePosition))
        candidates << candidatePosition;
    }
  }
  if (rateCallback.isCallable())
  {
    filterCases(game, rateCallback, candidates);
    compare = std::bind(&scriptedCandidateCompare, character, rateCallback, std::placeholders::_1, std::placeholders::_2);
  }
  else
    compare = std::bind(&candidateCompare, character, std::placeholders::_1, std::placeholders::_2);
  // Long story short: using std::sort with scriptedCandidateCompare ends up corrupting memory.
  // for some weird ass reason, this doesn't happen with std::stable_sort. What the fuck.
  // It is quite possible that different implementations of sort/stable_sort will exhibit different behaviours
  // in that regard. The vector returned by this function will probably cause issues on platforms other than Linux.
  std::stable_sort(candidates.begin(), candidates.end(), compare);
  return candidates;
}

int ReachAction::getApCost() const
{
  QVector<Point> candidates;

  if (alreadyReached())
    return 0;
  else if (range == 0.f)
    candidates.push_back(getTargetPosition());
  else
    candidates = getCandidates(static_cast<int>(std::floor(range)));
  return getApCostForCandidates(candidates);
}

int ReachAction::getApCostForCandidates(const QVector<Point> &candidates, bool quickMode) const
{
  QList<Point> path;
  auto& grid = Game::get()->getLevel()->getPathfinder();

  if (grid.findPath(character->getPoint(), candidates, path, character, quickMode))
    return pathApCost(path);
  return -1;
}

bool ReachAction::alreadyReached() const
{
  return std::floor(character->getDistance(object)) <= range && character->hasLineOfSight(object);
}

bool ReachAction::trigger()
{
  if (range == 0.f)
  {
    target = getTargetPosition();
    return MovementAction::trigger();
  }
  else if (alreadyReached())
    state = Done;
  else
  {
    auto& grid = Game::get()->getLevel()->getPathfinder();
    int caseDistance = static_cast<int>(std::floor(range));
    auto candidates = getCandidates(caseDistance);

    state = Interrupted;
    if (grid.findPath(character->getPoint(), candidates, character->rcurrentPath(), character))
      state = canMakeNextMovement() ? InProgress : Interrupted;
  }
  return state == Done || state == InProgress;
}

void ReachAction::triggerNextMovement()
{
  MovementAction::triggerNextMovement();
  if (state == Interrupted)
  {
    auto* level = Game::get()->getLevel();

    if (!level->isInCombat(character) || character->getActionPoints() > 0)
      trigger();
  }
}
