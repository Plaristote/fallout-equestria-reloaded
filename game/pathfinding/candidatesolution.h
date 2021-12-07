#ifndef  CANDIDATESOLUTION_H
# define CANDIDATESOLUTION_H

# include "levelgrid.h"
# include "astar.hpp"

class PathZone;
class ZoneGrid;

typedef AstarPathfinding<LevelGrid::CaseContent> CasePathfinder;

struct CandidateSolution
{
  enum State { Pending, Found, Failed };
  CandidateSolution(PathZone* a, LevelGrid::CaseContent* b) : zone(a), target(b) {}
  PathZone*               zone;
  LevelGrid::CaseContent* target;
  State                   state = Pending;
  QList<Point>            path;
};

struct CandidateSolutions : public QVector<CandidateSolution>
{
  CandidateSolutions(ZoneGrid& zoneGrid, const QVector<Point>& to);

  CandidateSolutions::const_iterator nextCandidate() const;
  CasePathfinder::CandidateList      candidateList(PathZone* zone) const;
  void                               addSolution(PathZone* zone, const QList<Point>& path);
};
#endif // CANDIDATESOLUTION_H
