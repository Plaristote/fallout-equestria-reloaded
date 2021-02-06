#include "levelgrid.h"
#include "tilemap/tilemap.h"
#include "dynamicobject.h"

LevelGrid::LevelGrid(QObject *parent) : QObject(parent)
{
}

void LevelGrid::initializeGrid(TileMap* tilemap)
{
  auto* wallLayer = tilemap->getLayer("walls");

  size = tilemap->getSize();
  for (int x = 0 ; x < size.width() ; ++x)
  {
    for (int y = 0 ; y < size.height() ; ++y)
    {
      CaseContent gridCase;

      gridCase.occupied = wallLayer->getTile(x, y) != nullptr;
      grid.push_back(gridCase);
    }
  }
}

LevelGrid::CaseContent* LevelGrid::getGridCase(int x, int y)
{
  int index = y * size.width() + x;

  if (index >= grid.count())
    return nullptr;
  return &(grid[index]);
}

bool LevelGrid::isOccupied(int x, int y) const
{
  int index = y * size.width() + x;

  if (index >= grid.count())
    return true;
  return grid.at(index).occupied;
}

void LevelGrid::setCaseOccupant(CaseContent& _case, DynamicObject* occupant)
{
  _case.occupied = occupant != nullptr;
  _case.occupant = occupant;
}

DynamicObject* LevelGrid::getOccupant(int x, int y)
{
  auto* gridCase = getGridCase(x, y);

  if (gridCase)
    return gridCase->occupant;
  return nullptr;
}

bool LevelGrid::moveObject(DynamicObject* object, int x, int y)
{
  auto* gridCase = getGridCase(x, y);

  if (gridCase && !gridCase->occupied)
  {
    QPoint currentPosition = object->getPosition();
    auto*  oldCase = getGridCase(currentPosition.x(), currentPosition.y());

    if (oldCase && oldCase->occupant == object)
      setCaseOccupant(*oldCase, nullptr);
    setCaseOccupant(*gridCase, object);
  }
  return false;
}
