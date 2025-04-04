#include "candidatesolution.h"
#include "zonegrid.h"

CandidateSolutions::CandidateSolutions(ZoneGrid& zoneGrid, const QVector<Point>& to)
{
  reserve(to.size());
  for (const Point candidateCase : to)
  {
    LevelGrid::CaseContent* destCase = zoneGrid.getGridCase(candidateCase);
    PathZone*               destZone = destCase ? zoneGrid.getPathZone(candidateCase) : nullptr;

    if (destCase && destZone)
      push_back(CandidateSolution(destZone, destCase));
  }
}

CandidateSolutions::const_iterator CandidateSolutions::nextCandidate() const
{
  auto it = begin();

  while (it != end() && it->state == CandidateSolution::Failed)
    it++;
  return it;
}

void CandidateSolutions::addSolution(PathZone* zone, const QList<Point>& path)
{
  for (auto it = begin() ; it != end() ; ++it)
  {
    if (it->zone->id == zone->id)
    {
      if (path.size() && it->target->position == path.last())
      {
        it->path = path;
        it->state = CandidateSolution::Found;
      }
      else
        it->state = CandidateSolution::Failed;
    }
  }
}

CasePathfinder::CandidateList CandidateSolutions::candidateList(PathZone* zone) const
{
  CasePathfinder::CandidateList list;

  for (auto it = begin() ; it != end() ; ++it)
  {
    if (it->zone->id == zone->id)
      list.push_back(*(it->target));
  }
  return list;
}
