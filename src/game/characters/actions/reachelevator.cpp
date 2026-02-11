#include "reachelevator.h"

Point ReachElevatorAction::getTargetPosition() const
{
  unsigned char floor = character->getCurrentFloor();
  QList<Point> candidates{
    elevator->getPositionA(),
    elevator->getPositionB()
  };

  for (Point candidate : candidates)
  {
    if (candidate.z == floor)
      return candidate;
  }
  return elevator->getPoint();
}
