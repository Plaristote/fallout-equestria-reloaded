#include "levelgrid.h"
#include "tilemap/tilemap.h"
#include "dynamicobject.h"
#include "astar.hpp"
#include <cmath>

LevelGrid::LevelGrid(QObject *parent) : QObject(parent)
{
}

void LevelGrid::initializeGrid(TileMap* tilemap)
{
  auto* wallLayer = tilemap->getLayer("walls");
  auto  zones     = tilemap->getZones();

  size = tilemap->getSize();
  grid.resize(size.width() * size.height());
  for (int x = 0 ; x < size.width() ; ++x)
  {
    for (int y = 0 ; y < size.height() ; ++y)
    {
      int position = y * size.width() + x;
      CaseContent& gridCase = grid[position];

      gridCase.occupied = wallLayer->getTile(x, y) != nullptr;
      gridCase.position = QPoint(x, y);
      for (auto* zone : zones)
      {
        if (zone->isInside(x, y))
        {
          gridCase.zone = zone;
          break ;
        }
      }
    }
  }
  initializePathfinding();
}

void LevelGrid::initializePathfinding()
{
  for (auto& gridCase : grid)
  {
    bool hasLeft  = gridCase.position.x() - 1 >= 0;
    bool hasRight = gridCase.position.x() + 1 < size.width();
    bool hasUp    = gridCase.position.y() - 1 >= 0;
    bool hasDown  = gridCase.position.y() + 1 < size.height();

    gridCase.successors.clear();
    gridCase.successors.push_back(hasLeft && hasUp    ? getGridCase(gridCase.position.x() - 1, gridCase.position.y() - 1) : nullptr);
    gridCase.successors.push_back(hasUp               ? getGridCase(gridCase.position.x(),     gridCase.position.y() - 1) : nullptr);
    gridCase.successors.push_back(hasRight && hasUp   ? getGridCase(gridCase.position.x() + 1, gridCase.position.y() - 1) : nullptr);
    gridCase.successors.push_back(hasLeft             ? getGridCase(gridCase.position.x() - 1, gridCase.position.y())     : nullptr);
    gridCase.successors.push_back(hasRight            ? getGridCase(gridCase.position.x() + 1, gridCase.position.y())     : nullptr);
    gridCase.successors.push_back(hasDown && hasLeft  ? getGridCase(gridCase.position.x() - 1, gridCase.position.y() + 1) : nullptr);
    gridCase.successors.push_back(hasDown             ? getGridCase(gridCase.position.x(),     gridCase.position.y() + 1) : nullptr);
    gridCase.successors.push_back(hasDown && hasRight ? getGridCase(gridCase.position.x() + 1, gridCase.position.y() + 1) : nullptr);
    for (auto it = gridCase.successors.begin() ; it != gridCase.successors.end() ;)
    {
      if ((*it) == nullptr || (*it)->occupied)
        it = gridCase.successors.erase(it);
      else
        it++;
    }
  }
}

bool LevelGrid::findPath(QPoint from, QPoint to, QList<QPoint>& path)
{
  typedef AstarPathfinding<LevelGrid::CaseContent> Pathfinder;
  Pathfinder        astar;
  unsigned short    iterationCount = 0;
  Pathfinder::State state;
  CaseContent*      fromCase = getGridCase(from.x(), from.y());
  CaseContent*      toCase   = getGridCase(to.x(), to.y());

  fromCase->occupied = false;
  path.clear();
  astar.SetStartAndGoalStates(*fromCase, *toCase);
  while ((state = astar.SearchStep()) == Pathfinder::Searching && ++iterationCount < 250);
  if (state == Pathfinder::Succeeded)
  {
    for (auto& gridCase : astar.GetSolution())
      path << gridCase.position;
    path.pop_front(); // first case is the starting point
    fromCase->occupied = true;
    return true;
  }
  fromCase->occupied = true;
  return false;
}

std::list<LevelGrid::CaseContent*> LevelGrid::CaseContent::GetSuccessors(const CaseContent* parent) const
{
  std::list<LevelGrid::CaseContent*> results;

  for (auto* node : successors)
  {
    if (node != parent && !node->occupied)
      results.push_back(node);
  }
  return results;
}

float LevelGrid::CaseContent::GoalDistanceEstimate(const CaseContent& other) const
{
  int distX = position.x() - other.position.x();
  int distY = position.y() - other.position.y();

  return std::sqrt(static_cast<float>(distX * distX + distY * distY));
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

void LevelGrid::removeObject(DynamicObject* object)
{
  auto  position = object->getPosition();
  auto* gridCase = getGridCase(position.x(), position.y());

  if (gridCase && gridCase->occupant == object)
  {
    setCaseOccupant(*gridCase, nullptr);
    emit gridCase->zone->exitedZone(object, gridCase->zone);
  }
}

bool LevelGrid::moveObject(DynamicObject* object, int x, int y)
{
  auto* gridCase = getGridCase(x, y);

  if (gridCase && !gridCase->occupied)
  {
    QPoint currentPosition = object->getPosition();
    auto*  oldCase = getGridCase(currentPosition.x(), currentPosition.y());

    if (oldCase && oldCase->occupant == object)
    {
      setCaseOccupant(*oldCase, nullptr);
      if (oldCase->zone && oldCase->zone != gridCase->zone)
        emit oldCase->zone->exitedZone(object, oldCase->zone);
    }
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

void LevelGrid::triggerZone(DynamicObject* object, int x, int y)
{
  auto* gridCase = getGridCase(x, y);

  if (gridCase && gridCase->zone && gridCase->zone->getName() != object->getCurrentZone())
  {
    object->setCurrentZone(gridCase->zone->getName());
    emit gridCase->zone->enteredZone(object, gridCase->zone);
  }
  else if (object->getCurrentZone() != "" && (!gridCase || !gridCase->zone))
    object->setCurrentZone("");
}
