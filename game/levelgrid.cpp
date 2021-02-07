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
  grid.resize(size.width() * size.height());
  for (int x = 0 ; x < size.width() ; ++x)
  {
    for (int y = 0 ; y < size.height() ; ++y)
    {
      int position = y * size.width() + x;
      CaseContent& gridCase = grid[position];

      gridCase.occupied = wallLayer->getTile(x, y) != nullptr;
    }
  }
}

LevelGrid::CaseContent* LevelGrid::getGridCase(int x, int y)
{
  int position = y * size.width() + x;

  if (position >= grid.count())
    return nullptr;
  return &(grid[position]);
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
    return true;
  }
  else if (!gridCase)
    qDebug() << "LevelRange: Out of range" << x << y;
  else if (gridCase->occupant == nullptr)
    qDebug() << "LevelGrid:" << x << y << "haz a wall";
  else
    qDebug() << "LevelGrid:" << x << y << "already occupied";
  return false;
}
