#include "levelgrid.h"
#include "../level/grid.h"
#include "tilemap/tilemap.h"
#include "../dynamicobject.h"
#include "../character.h"
#include <QLineF>
#include <QRectF>
#include <QDebug>

LevelGrid::LevelGrid(QObject *parent) : QObject(parent)
{
}

LevelGrid::~LevelGrid()
{
  for (auto& caseContent : grid)
    caseContent.clearConnections();
}

void LevelGrid::updateZoneCases(TileZone* zone)
{
  setZoneCases(zone, zone->getAbsolutePositions());
}

void LevelGrid::setZoneCases(TileZone* zone, QVector<QPoint> positions)
{
  QVector<CaseContent*>& cases = zoneCases[zone];

  for (auto it = cases.begin() ; it != cases.end() ;)
  {
    if (positions.contains((*it)->position))
    {
      positions.removeAll((*it)->position);
      ++it;
    }
    else
    {
      (*it)->zones.removeOne(zone);
      it = cases.erase(it);
    }
  }
  for (QPoint position : positions)
  {
    CaseContent* gridCase = getGridCase(position.x(), position.y());

    if (gridCase)
    {
      gridCase->zones.push_back(zone);
      cases.push_back(gridCase);
    }
  }
}

void LevelGrid::eachCase(std::function<void (int x, int y, CaseContent&)> callback, QPoint from, QPoint to)
{
  if (to.x() == 0 && to.y() == 0)
    to = QPoint(size.width(), size.height());
  for (int x = from.x() ; x < to.x() ; ++x)
  {
    for (int y = from.y() ; y < to.y() ; ++y)
    {
      int          position = y * size.width() + x;
      CaseContent& gridCase = grid[position];

      callback(x, y, gridCase);
    }
  }
}

void LevelGrid::registerZone(TileZone* zone)
{
  if (!zoneCases.contains(zone))
  {
    auto listener = connect(zone, &TileZone::tilesChanged, [this, zone]() { updateZoneCases(zone); });

    zoneListener.insert(zone, listener);
    zoneCases.insert(zone, {});
    updateZoneCases(zone);
  }
}

void LevelGrid::unregisterZone(TileZone* zone)
{
  if (zoneCases.contains(zone))
  {
    auto listener = zoneListener.find(zone);

    if (listener != zoneListener.end())
    {
      disconnect(*listener);
      zoneListener.erase(listener);
    }
    setZoneCases(zone, {});
    zoneCases.remove(zone);
  }
}

LevelGrid::CaseContent* LevelGrid::getGridCase(int x, int y)
{
  int position = y * size.width() + x;

  if (position >= grid.count() || position < 0)
    return nullptr;
  return &(grid[position]);
}

LevelGrid* getFloorGrid(unsigned char floor);

LevelGrid::CaseContent* LevelGrid::getGridCase(int x, int y, unsigned char z)
{
  LevelGrid* targetGrid = this;

  if (z != getTilemap()->getFloor())
    targetGrid = getFloorGrid(z);
  return targetGrid ? targetGrid->getGridCase(x, y) : nullptr;
}

QList<TileZone*> LevelGrid::getZones() const
{
  return zoneListener.keys();
}

QVector<TileZone*> LevelGrid::getZonesAt(QPoint position)
{
  auto* gridCase = getGridCase(position.x(), position.y());

  if (gridCase)
    return gridCase->zones;
  return QVector<TileZone*>();
}

bool LevelGrid::isOccupied(int x, int y) const
{
  int index = y * size.width() + x;

  if (index >= grid.count() || index < 0)
    return true;
  return grid.at(index).isBlocked();
}

int LevelGrid::getCaseFlags(int x, int y) const
{
  int index = y * size.width() + x;
  int flag = 0;

  if (index >= grid.count() || index < 0)
    return BlockCase;
  const CaseContent& gridCase = grid.at(index);

  if (gridCase.block)
    flag += BlockCase;
  if (gridCase.vwall)
    flag += VerticalWall;
  if (gridCase.hwall)
    flag += HorizontalWall;
  return flag;
}

int LevelGrid::getCoverValue(int x, int y) const
{
  int position = y * size.width() + x;
  const CaseContent* gridCase;

  if (position >= grid.count() || position < 0)
    return 0;
  gridCase = &(grid[position]);
  return gridCase->cover;
}

void LevelGrid::setCaseOccupant(CaseContent& _case, DynamicObject* occupant)
{
  if (occupant && occupant->isBlockingPath())
  {
    _case.occupied = true;
    _case.occupant = occupant;
  }
  else
  {
    _case.occupied = false;
    _case.occupant = nullptr;
  }
}

DynamicObject* LevelGrid::getOccupant(int x, int y)
{
  auto* gridCase = getGridCase(x, y);

  if (gridCase)
  {
    if (gridCase->occupant)
      return gridCase->occupant;
    for (auto* zone : qAsConst(gridCase->zones))
    {
      if (zone->getAccessBlocked() && zone->getOwner())
        return zone->getOwner();
    }
  }
  return nullptr;
}

void LevelGrid::removeObject(DynamicObject* object)
{
  auto  position = object->getPosition();
  auto* gridCase = getGridCase(position.x(), position.y());

  if (gridCase)
  {
    extractObject(object);
    for (auto* zone : qAsConst(gridCase->zones))
    {
      if (object->isCharacter())
        reinterpret_cast<Character*>(object)->onZoneExited(zone);
      emit zone->exitedZone(object, zone);
    }
  }
}

void LevelGrid::extractObject(DynamicObject* object)
{
  QPoint currentPosition = object->getPosition();
  auto*  oldCase = getGridCase(currentPosition.x(), currentPosition.y());

  if (oldCase && oldCase->occupant == object)
    setCaseOccupant(*oldCase, nullptr);
}

bool LevelGrid::insertObject(DynamicObject* object, int x, int y)
{
  auto* gridCase = getGridCase(x, y);

  if (gridCase)
  {
    if (object->isBlockingPath())
      setCaseOccupant(*gridCase, object);
    object->setCurrentFloor(static_cast<unsigned char>(tilemap->getFloor()));
    object->setPosition(QPoint(x, y));
    updateObjectVisibility(object);
    return true;
  }
  return false;
}

bool LevelGrid::moveObject(DynamicObject* object, int x, int y)
{
  extractObject(object);
  return insertObject(object, x, y);
}

void LevelGrid::updateObjectVisibility(DynamicObject* object)
{
  TileLayer* roof = reinterpret_cast<GridComponent*>(parent())->getRoofFor(object);

  object->setVisible(!roof || !roof->isVisible());
}

static const QVector<TileZone*> emptyZoneList;

void LevelGrid::triggerZone(DynamicObject* object, int x, int y)
{
  if (!object->isCharacter())
  {
    auto* gridCase = getGridCase(x, y);
    const QVector<TileZone*>& newZones  = gridCase ? gridCase->zones : emptyZoneList;

    for (auto* zone : newZones)
      emit zone->enteredZone(object, zone);
  }
  else
    triggerZone(reinterpret_cast<CharacterMovement*>(object), x, y);
}

void LevelGrid::triggerZone(CharacterMovement* character, int x, int y)
{
  auto* gridCase = getGridCase(x, y);
  const QVector<TileZone*>  lastZones = character->getCurrentZones();
  const QVector<TileZone*>& newZones  = gridCase ? gridCase->zones : emptyZoneList;

  for (auto* zone : newZones)
  {
    if (!(character->isInZone(zone)))
      emit zone->enteredZone(character, zone);
  }
  for (auto* zone : lastZones)
  {
    if (!(zone->isInside(x, y, static_cast<unsigned char>(character->getCurrentFloor()))))
      emit zone->exitedZone(character, zone);
  }
}
