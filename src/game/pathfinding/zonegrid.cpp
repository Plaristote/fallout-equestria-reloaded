#include "zonegrid.h"
#include "candidatesolution.h"
#include "tilemap/tilemap.h"

ZoneGrid::ZoneGrid()
{

}

LevelGrid::CaseContent* ZoneGrid::getGridCase(Point position)
{
  for (LevelGrid* level : levels)
  {
    if (level->getTilemap()->getFloor() == position.z)
      return level->getGridCase(position.x, position.y);
  }
  return nullptr;
}

PathZone* ZoneGrid::getPathZone(Point position)
{
  for (PathZone& zone : zones)
  {
    if (zone.contains(position))
      return &zone;
  }
  return nullptr;
}

PathZone* ZoneGrid::getPathZoneById(unsigned int id)
{
  auto it = std::find(zones.begin(), zones.end(), id);

  return it == zones.end() ? nullptr : &(*it);
}

void ZoneGrid::connectCases(Point a, Point b)
{
  auto* caseA = getGridCase(a);
  auto* zoneA = getPathZone(a);
  auto* caseB = getGridCase(b);
  auto* zoneB = getPathZone(b);

  if (caseA && caseB && zoneA && zoneB && caseA->connectionWith(caseB) == nullptr)
  {
    caseA->connectWith(caseB);
    if (zoneA != zoneB)
    {
      zoneA->addExitTowards(zoneB, {caseA, caseB});
      zoneB->addExitTowards(zoneA, {caseB, caseA});
    }
  }
}

void ZoneGrid::disconnectCases(Point a, Point b)
{
  auto* caseA = getGridCase(a);
  auto* zoneA = getPathZone(a);
  auto* caseB = getGridCase(b);
  auto* zoneB = getPathZone(b);

  if (caseA && caseB && caseA->connectionWith(caseB) != nullptr)
  {
    caseA->disconnectFrom(caseB);
    if (zoneA != zoneB)
    {
      zoneA->removeExitTowards(zoneB, {caseA, caseB});
      zoneB->removeExitTowards(zoneA, {caseB, caseA});
    }
  }
}

bool ZoneGrid::findPath(Point from, Point to, QList<Point>& path, CharacterMovement* character)
{
  return findPath(from, QVector<Point>{to}, path, character);
}

int ZoneGrid::actionPointCost(Point a, Point b)
{
  const auto* caseA = getGridCase(a);
  const auto* caseB = getGridCase(b);

  return caseA->apCostTo(caseB);
}
