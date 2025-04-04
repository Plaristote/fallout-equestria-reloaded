#ifndef  CASELOCKER_H
# define CASELOCKER_H

# include "levelgrid.h"

struct CaseLocker
{
  LevelGrid::CaseContent* gridCase;
  bool                    backup;
  CaseLocker(LevelGrid::CaseContent* gridCase) : gridCase(gridCase) { if (gridCase) { backup = gridCase->occupied; } }
  ~CaseLocker() { if (gridCase) { gridCase->occupied = backup; } }
};

#endif // CASELOCKER_H
