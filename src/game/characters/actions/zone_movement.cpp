#include "zone_movement.h"
#include <QRandomGenerator>
#include "game/leveltask.h"
#include "game/pathfinding/levelgrid.h"

bool ZoneMovementAction::trigger()
{
  QRandomGenerator* random = QRandomGenerator::global();
  LevelGrid*        floorGrid = LevelTask::get()->getFloorGrid(zone->getFloor());
  QList<QPoint>     candidates = zone->getPositions();

  while (candidates.size())
  {
    int it = random->bounded(0, candidates.size());
    QPoint position = candidates[it];

    if (!floorGrid->isOccupied(position.x(), position.y()))
    {
      target = Point{position.x(), position.y(), static_cast<unsigned char>(zone->getFloor())};
      return MovementAction::trigger();
    }
    candidates.removeAt(it);
  }
  return false;
}
