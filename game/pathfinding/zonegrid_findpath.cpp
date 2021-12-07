#include "zonegrid.h"
#include "candidatesolution.h"
#include "caselocker.h"
#include "tilemap/tilemap.h"

typedef AstarPathfinding<LevelGrid::CaseContent>  CasePathfinder;
typedef AstarPathfinding<PathZone>                ZonePathfinder;
typedef QPair<PathZone*, LevelGrid::CaseContent*> Destination;
typedef std::function<bool(const LevelGrid::CaseContent&, const LevelGrid::CaseContent&)> CaseSorter;

static bool sortCasesByProximity(
    const LevelGrid::CaseContent& goal,
    const LevelGrid::CaseContent& a,
    const LevelGrid::CaseContent& b)
{
  return a.GoalDistanceEstimate(goal) < b.GoalDistanceEstimate(goal);
}

static CasePathfinder::CandidateList sortedCaseCandidates(const PathZone::Connections& connections, CaseSorter heuristic)
{
  std::vector<LevelGrid::CaseContent> sortableCandidates;

  for (const auto connection : connections)
    sortableCandidates.push_back(*connection.second);
  std::sort(sortableCandidates.begin(), sortableCandidates.end(), heuristic);
  return CasePathfinder::CandidateList(sortableCandidates.begin(), sortableCandidates.end());
}

static bool findPathToZone(
    PathZone*               fromZone,
    LevelGrid::CaseContent* fromCase,
    PathZone*               toZone,
    CaseSorter              heuristic,
    QList<Point>&           path,
    CharacterMovement*      character)
{
  auto* exits = fromZone->getExitsTowards(toZone);

  if (exits && exits->size() > 0)
  {
    CasePathfinder                astar(character);
    CasePathfinder::Solution*     solution;
    CasePathfinder::CandidateList candidates = sortedCaseCandidates(*exits, heuristic);
    unsigned short                iteractionCount = 0;

    astar.SetStartAndGoalStates(*fromCase, candidates);
    astar.scope = [fromZone](const LevelGrid::CaseContent& gridCase) { return fromZone->contains(gridCase.position) || fromZone->neighbors(gridCase.position); };
    astar.AcceptAnyCandidate();
    while (astar.SearchStep() == CasePathfinder::Searching && ++iteractionCount < 250);
    solution = astar.GetBestSolution();
    if (solution)
    {
      for (auto it = ++solution->begin() ; it != solution->end() ; ++it)
        path << it->position;
      return true;
    }
    else
      qDebug() << "-> findPathToZone found an exit towards target zone, but couldn't reach it.";
  }
  else
    qDebug() << "-> findPathToZone found no available exits towards target zone";
  return false;
}

static bool findZonePathToZone(
    ZoneGrid&               zoneGrid,
    PathZone*               fromZone,
    LevelGrid::CaseContent* fromCase,
    PathZone*               toZone,
    CaseSorter              heuristic,
    QList<Point>&           path,
    CharacterMovement*      character
    )
{
  ZonePathfinder            astar(character);
  ZonePathfinder::Solution* solution;
  unsigned short            iterationCount = 0;

  qDebug() << "-> AStar looking for path between" << fromZone->rect << "and" << toZone->rect;
  astar.SetStartAndGoalStates(*fromZone, {*toZone});
  while (astar.SearchStep() == ZonePathfinder::Searching && iterationCount < 250);
  solution = astar.GetBestSolution();
  if (solution)
  {
    auto* stepState = fromCase;
    auto  stepIt    = solution->begin();
    auto* stepZone  = zoneGrid.getPathZoneById(solution->begin()->id);

    if (solution->size() > 1)
     stepIt++;
    while (stepIt != solution->end())
    {
      auto* nextZone = zoneGrid.getPathZoneById(stepIt->id);

      if (findPathToZone(stepZone, stepState, nextZone, heuristic, path, character))
      {
        stepState = zoneGrid.getGridCase(path.last());
        stepZone  = nextZone;
        stepIt++;
      }
      else
        break ;
    }
    return stepIt == solution->end();
  }
  else
    qDebug() << "-> AStar claims there's no way";
  return false;
}

bool ZoneGrid::findPath(Point from, const QVector<Point> &to, QList<Point> &path, CharacterMovement *character, bool quickMode)
{
  LevelGrid::CaseContent*            fromCase   = getGridCase(from);
  LevelGrid::CaseContent*            targetCase = to.size() > 0 ? getGridCase(to.first()) : nullptr;
  PathZone*                          fromZone   = getPathZone(from);
  CandidateSolutions                 candidates(*this, to);
  CandidateSolutions::const_iterator candidate;
  CaseSorter                         heuristic = std::bind(&sortCasesByProximity, *targetCase, std::placeholders::_1, std::placeholders::_2);
  CaseLocker                         caseLock(fromCase);

  qDebug() << character << "findPath" << candidates.length();
  while ((candidate = candidates.nextCandidate()) != candidates.end())
  {
    if (candidate->state == CandidateSolution::Pending)
    {
      QList<Point> currentPath;
      bool         failed = false;

      qDebug() << "-> trying candidate Zone" << candidate->zone->id << " Position" << candidate->target->position;
      if (fromZone->id != candidate->zone->id)
      {
        qDebug() << "-> finding path towards zone";
        failed = !findZonePathToZone(*this,
                                        fromZone, fromCase, candidate->zone,
                                        heuristic, currentPath, character);
      }
      else
        qDebug() << "-> skipping zone pathvinding (target is in start zone)";
      if (!failed)
      {
        CasePathfinder            astar(character);
        CasePathfinder::Solution* solution;
        LevelGrid::CaseContent*   stepCase = currentPath.size() ? getGridCase(currentPath.last()) : fromCase;
        unsigned short            iterationCount = 0;

        astar.scope = [candidate](const LevelGrid::CaseContent& gridCase) { return candidate->zone->contains(gridCase.position) || candidate->zone->neighbors(gridCase.position); };
        astar.SetStartAndGoalStates(*stepCase, candidates.candidateList(candidate->zone));
        if (quickMode)
          astar.AcceptAnyCandidate();
        while (astar.SearchStep() == CasePathfinder::Searching && ++iterationCount < 250);
        solution = astar.GetBestSolution();
        if (solution)
        {
          for (auto it = ++solution->begin() ; it != solution->end() ; ++it)
            currentPath.push_back(it->position);
          qDebug() << "-> path to target case found" << currentPath.size();
        }
        else
        {
          qDebug() << "-> /!\\ path to target case not found";
          failed = true;
        }
      }
      else
        qDebug() << "-> /!\\ path to zone not found";
      candidates.addSolution(candidate->zone, !failed ? currentPath : QList<Point>());
    }
    else
    {
      path = candidate->path;
      qDebug() << "(!) Pathfinding success. Path size:" << path.size();
      return true;
    }
  }
  qDebug() << "(!) Pathfinding failure...";
  return false;
}
