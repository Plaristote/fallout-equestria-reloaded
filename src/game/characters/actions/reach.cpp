#include "reach.h"
#include "game/leveltask.h"
#include <cmath>
#include <functional>
#define MAX_RANGE 100

static int rateCase(QJSValue callback, const Point position)
{
  return ScriptController::callFunction(callback, QJSValueList() << position.x << position.y << static_cast<int>(position.z)).toInt();
}

static void filterCases(QJSValue callback, QVector<Point>& candidates)
{
  for (auto it = candidates.begin() ; it != candidates.end() ;)
  {
    if (rateCase(callback, *it) < 0)
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
  int rateA = rateCase(callback, a);
  int rateB = rateCase(callback, b);

  if (rateA != rateB)
    return rateA < rateB;
  return candidateCompare(character, a, b);
}

inline static LevelGrid* getGrid(unsigned int z)
{
  auto* level = LevelTask::get();

  return level ? level->getFloorGrid(z) : nullptr;
}

QVector<Point> ReachAction::getCandidates(int caseDistance) const
{
  Point position = getTargetPosition();
  QVector<Point> candidates;
  std::function<bool (Point, Point)> compare;
  LevelGrid* grid;

  if (caseDistance > MAX_RANGE)
    return candidates;
  candidates.reserve(caseDistance * caseDistance);
  grid = getGrid(position.z);
  for (int x = position.x - caseDistance ; x <= position.x + caseDistance ; ++x)
  {
    for (int y = position.y - caseDistance ; y <= position.y + caseDistance ; ++y)
    {
      Point candidatePosition{x, y, position.z};

      if (candidatePosition != position && !grid->isOccupied(candidatePosition.x, candidatePosition.y) && character->hasSightFrom(position, candidatePosition))
        candidates << candidatePosition;
    }
  }
  if (rateCallback.isCallable())
  {
    filterCases(rateCallback, candidates);
    compare = std::bind(&scriptedCandidateCompare, character, rateCallback, std::placeholders::_1, std::placeholders::_2);
  }
  else
    compare = std::bind(&candidateCompare, character, std::placeholders::_1, std::placeholders::_2);
  std::sort(candidates.begin(), candidates.end(), compare);
  return candidates;
}

int ReachAction::getApCost() const
{
  QVector<Point> candidates;

  if (!forced && alreadyReached())
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
  auto& grid = LevelTask::get()->getPathfinder();

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
  else if (!forced && alreadyReached())
    state = Done;
  else
  {
    auto& grid = LevelTask::get()->getPathfinder();
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
    auto* level = LevelTask::get();

    if (!level->isInCombat(character) || character->getActionPoints() > 0)
      trigger();
  }
}
