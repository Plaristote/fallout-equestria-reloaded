#include "reach.h"
#include "game.h"
#include <cmath>

static int rateCase(Game* game, QJSValue callback, QPoint position)
{
  return game->scriptCall(callback, QJSValueList() << position.x() << position.y(), "ReachAction").toInt();
}

static void filterCases(Game* game, QJSValue callback, QVector<QPoint>& candidates)
{
  std::remove_if(candidates.begin(), candidates.end(), [game, callback](QPoint position)
  {
    return rateCase(game, callback, position) < 0;
  });
}

static bool candidateCompare(Character* character, QPoint a, QPoint b)
{
  return character->getDistance(a) < character->getDistance(b);
}

static bool scriptedCandidateCompare(Character* character, QJSValue callback, QPoint a, QPoint b)
{
  Game* game = Game::get();
  int   rateA = rateCase(game, callback, a);
  int   rateB = rateCase(game, callback, b);

  if (rateA < rateB)
    return true;
  return candidateCompare(character, a, b);
}

QVector<QPoint> ReachAction::getCandidates(int caseDistance) const
{
  QPoint position = getTargetPosition();
  QVector<QPoint> candidates;
  Game* game = Game::get();
  std::function<bool (QPoint, QPoint)> compare;

  candidates.reserve(caseDistance * caseDistance);
  for (int x = position.x() - caseDistance ; x <= position.x() + caseDistance ; ++x)
  {
    for (int y = position.y() - caseDistance ; y <= position.y() + caseDistance ; ++y)
    {
      if (x != position.x() || y != position.y())
        candidates << QPoint(x, y);
    }
  }
  if (rateCallback.isCallable())
  {
    filterCases(game, rateCallback, candidates);
    compare = std::bind(&scriptedCandidateCompare, character, rateCallback, std::placeholders::_1, std::placeholders::_2);
  }
  else
    compare = std::bind(&candidateCompare, character, std::placeholders::_1, std::placeholders::_2);
  std::sort(candidates.begin(), candidates.end(), compare);
  return candidates;
}

int ReachAction::getApCost() const
{
  QVector<QPoint> candidates;
  auto* level = Game::get()->getLevel();
  auto* grid  = level->getGrid();

  if (alreadyReached())
    return 0;
  else if (range == 0.f)
    candidates.push_back(getTargetPosition());
  else
    candidates = getCandidates(static_cast<int>(std::floor(range)));
  for (auto it = candidates.begin() ; it != candidates.end() ; ++it)
  {
    QList<QPoint> path;

    if (grid->findPath(character->getPosition(), *it, path, character))
      return path.size();
  }
  return -1;
}

bool ReachAction::alreadyReached() const
{
  return character->getDistance(object) <= range && character->hasLineOfSight(object);
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
    auto* level = Game::get()->getLevel();
    auto* grid  = level->getGrid();
    int caseDistance = static_cast<int>(std::floor(range));
    auto candidates = getCandidates(caseDistance);

    state = Interrupted;
    for (auto it = candidates.begin() ; it != candidates.end() ; ++it)
    {
      if (grid->findPath(character->getPosition(), *it, character->rcurrentPath(), character))
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
  {
    auto* level = Game::get()->getLevel();

    if (!level->isInCombat(character) || character->getActionPoints() > 0)
      trigger();
  }
}
