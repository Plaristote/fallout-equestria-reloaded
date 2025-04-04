#include "zonegrid.h"
#include <cmath>
#include <QDebug>
#include "tilemap/tilemap.h"
#define ZONE_GRANULARITY 10

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

static void adjustZoneSize(QRect& zoneSize, QPoint position)
{
  if (position.x() < zoneSize.x())
    zoneSize.setLeft(position.x());
  else if (position.x() > zoneSize.x())
    zoneSize.setRight(position.x());
  if (position.y() < zoneSize.y())
    zoneSize.setTop(position.y());
  else if (position.y() > zoneSize.bottom())
    zoneSize.setBottom(position.y());
}

static QVector<LevelGrid::CaseContent*> getCaseSubset(const QVector<LevelGrid::CaseContent*>& cases, QRect rect)
{
  QVector<LevelGrid::CaseContent*> subset;

  for (auto it = cases.begin() ; it != cases.end() ; ++it)
  {
    if (rect.contains((*it)->position))
      subset.push_back(*it);
  }
  return subset;
}

static QVector<PathZone> subdivicePathZoneFromLayer(QRect zoneSize, QVector<LevelGrid::CaseContent*>& cases)
{
  QVector<PathZone> zones;
  QVector<QRect>    parts;
  const int         granularityInt = ZONE_GRANULARITY;
  const double      granularity = static_cast<double>(granularityInt);
  const QPoint      zoneBaseSize(granularityInt, granularityInt);

  for (int x = 0 ; x < std::ceil(zoneSize.width() / granularity) ; ++x)
  {
    for (int y = 0 ; y < std::ceil(zoneSize.height() / granularity) ; ++y)
    {
      QPoint   topLeft(x * granularityInt, y * granularityInt);

      parts << QRect(topLeft, topLeft + zoneBaseSize);
    }
  }
  for (const auto& part : parts)
    zones << makePathZonesFrom(getCaseSubset(cases, part));
  return zones;
}

static QVector<PathZone> preparePathZoneFromLayer(LevelGrid* grid, const TileZone* source, QVector<LevelGrid::CaseContent*>& allCases)
{
  QVector<LevelGrid::CaseContent*> cases;
  const auto& positions = source->getPositions();
  QRect zoneSize;
  int granularity = source->getGranularity();

  if (allCases.size() > 1)
  {
    QPoint origin(allCases.first()->position);
    zoneSize = QRect(origin, origin);
  }
  for (auto it = allCases.begin() ; it != allCases.end() ;)
  {
    auto* gridCase = *it;

    adjustZoneSize(zoneSize, gridCase->position);
    if (positions.indexOf(gridCase->position) >= 0)
    {
      cases.push_back(gridCase);
      it = allCases.erase(it);
    }
    else
      ++it;
  }
  if (granularity == 0)
    granularity = ZONE_GRANULARITY;
  if (granularity > 0 && (zoneSize.width() > granularity || zoneSize.height() > granularity))
    return subdivicePathZoneFromLayer(zoneSize, cases);
  return makePathZonesFrom(cases);
}

static void prepareZoneGridForFloorUsingTilemap(ZoneGrid& zoneGrid, LevelGrid* grid)
{
  const auto& mapZones = grid->getTilemap()->getPathfindindingZones();
  QVector<LevelGrid::CaseContent*> allCases;

  for (int x = 0 ; x < grid->getSize().width() ; ++x)
  {
    for (int y = 0 ; y < grid->getSize().height() ; ++y)
    {
      auto* gridCase = grid->getGridCase(x, y);

      if (gridCase && gridCase->connections.size())
        allCases.push_back(gridCase);
    }
  }
  for (const auto* mapZone : mapZones)
    zoneGrid.zones << preparePathZoneFromLayer(grid, mapZone, allCases);
  if (allCases.size() > 0)
    zoneGrid.zones << makePathZonesFrom(allCases);
}

static void prepareZoneGridForFloor(ZoneGrid& zoneGrid, LevelGrid* grid)
{
  QVector<QRect> parts;
  const int      granularityInt = ZONE_GRANULARITY;
  const double   granularity = static_cast<double>(granularityInt);
  const QPoint   zoneBaseSize(granularityInt, granularityInt);

  for (int x = 0 ; x < std::ceil(grid->getSize().width() / granularity) ; ++x)
  {
    for (int y = 0 ; y < std::ceil(grid->getSize().height() / granularity) ; ++y)
    {
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
  {
    if (grid->hasPathfindingZones())
      prepareZoneGridForFloorUsingTilemap(*this, grid);
    else
      prepareZoneGridForFloor(*this, grid);
  }
  for (PathZone& zone : zones)
  {
    zone.id = n++;
    connectPathZones(zone, zones);
  }
}
