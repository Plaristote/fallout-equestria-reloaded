#include "zonegrid.h"
#include <cmath>
#include <QDebug>
#include "tilemap/tilemap.h"

static void detectZoneBoundaries(PathZone& zone)
{
  QPoint topLeft, bottomRight;
  auto it = zone.positions.begin();

  topLeft = bottomRight = *it;
  while (++it != zone.positions.end())
  {
    if (it->x() < topLeft.x())
      topLeft.setX(it->x());
    if (it->y() < topLeft.y())
      topLeft.setY(it->y());
    if (it->x() > bottomRight.x())
      bottomRight.setX(it->x());
    if (it->y() > bottomRight.y())
      bottomRight.setY(it->y());
  }
  zone.rect = QRect(topLeft, bottomRight);
}

static QVector<PathZone> makePathZonesFrom(QVector<LevelGrid::CaseContent*> missingCases)
{
  PathZone zone;
  QVector<PathZone> zoneList;
  QVector<LevelGrid::CaseContent*> aggregatedCases;

  if (missingCases.size() > 0)
  {
    // - start by registering the initial case
    // - aggregatedCases will then be progressively filled with explorable nodes
    zone.positions.push_back(missingCases.first()->position);
    zone.floor = missingCases.first()->position.z;
    aggregatedCases.push_back(*missingCases.begin());
    missingCases.pop_front();
    for (int caseIt = 0 ; caseIt < aggregatedCases.size() ; ++caseIt)
    {
      auto* currentCase = aggregatedCases[caseIt];

      // - loop over all the cases connected to the currentCase
      // - check if each candidate was part of the collected cases in step1
      // - if they are, add it to the explorable list, and remove it from the list o cases to check
      for (auto* caseConnection : currentCase->connections)
      {
        auto* candidateCase = caseConnection->getTargetFor(currentCase);
        auto it = std::find(missingCases.begin(), missingCases.end(), candidateCase);

        if (it != missingCases.end())
        {
          zone.positions.push_back(candidateCase->position);
          aggregatedCases.push_back(candidateCase);
          missingCases.erase(it);
        }
      }
    }

    // - loop over all the aggregated cases again
    // - find connections to cases that are not aggregated
    // - register these cases as connections to other zones
    for (auto* currentCase : aggregatedCases)
    {
      for (auto* caseConnection : currentCase->connections)
      {
        auto* candidateCase = caseConnection->getTargetFor(currentCase);
        auto it = std::find(aggregatedCases.begin(), aggregatedCases.end(), candidateCase);

        if (it == aggregatedCases.end())
          zone.exits.push_back({currentCase, candidateCase});
      }
    }
    detectZoneBoundaries(zone);
    if (zone.positions.size() > 1 || zone.exits.size() > 0)
      zoneList << zone;
    zoneList << makePathZonesFrom(missingCases);
  }
  return zoneList;
}

static QVector<PathZone> subdivideZoneGrid(QRect rect, LevelGrid* grid)
{
  QVector<LevelGrid::CaseContent*> cases;
  TileLayer* ground = grid->getTilemap()->getLayer("ground");

  // step 1: register all the cases within the zone
  for (int x = rect.left() ; x < rect.right() ; ++x)
  {
    for (int y = rect.top() ; y < rect.bottom() ; ++y)
    {
      auto* gridCase = grid->getGridCase(x, y);

      if (gridCase && gridCase->connections.size())
        cases.push_back(gridCase);
    }
  }
  return makePathZonesFrom(cases);
}

static void preparZoneGridForFloor(ZoneGrid& zoneGrid, LevelGrid* grid)
{
  QVector<QRect> parts;
  const int      granularityInt = 10;
  const double   granularity = static_cast<double>(granularityInt);
  const QPoint   zoneBaseSize(granularityInt, granularityInt);

  for (int x = 0 ; x < std::ceil(grid->getSize().width() / granularity) ; ++x)
  {
    for (int y = 0 ; y < std::ceil(grid->getSize().height() / granularity) ; ++y)
    {
      PathZone zone;
      QPoint   topLeft(x * granularityInt, y * granularityInt);

      parts << QRect(topLeft, topLeft + zoneBaseSize);
    }
  }
  for (const auto& part : parts)
    zoneGrid.zones << subdivideZoneGrid(part, grid);
}

static void connectPathZones(PathZone& zone, QVector<PathZone>& zones)
{
  for (const PathZone::Connection& exit : zone.exits)
  {
    for (PathZone& candidate : zones)
    {
      if (candidate.contains(exit.second->position))
      {
        auto it = zone.connections.find(&candidate);

        if (it == zone.connections.end())
          zone.connections.insert(&candidate, {exit});
        else
          it->push_back(exit);
      }
    }
  }
}

void ZoneGrid::prepareZoneGrid(QVector<LevelGrid*> grids)
{
  unsigned int n = 1;

  levels = grids;
  for (LevelGrid* grid : grids)
    preparZoneGridForFloor(*this, grid);
  for (PathZone& zone : zones)
  {
    zone.id = n++;
    connectPathZones(zone, zones);
  }
}