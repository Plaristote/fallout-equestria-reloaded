#include "levelgrid.h"
#include "level/grid.h"
#include "tilemap/tilemap.h"
#include "dynamicobject.h"
#include "character.h"
#include "objects/doorway.h"
#include "astar.hpp"
#include <cmath>
#include <QLineF>
#include <QRectF>
#include <QDebug>

static bool isWall(const TileLayer* layer, int x, int y)
{
  if (layer)
  {
    Tile* tile = layer->getTile(x, y);

    if (tile)
    {
      QVariant doorwayProp = tile->getProperty("doorway");

      return doorwayProp.isNull() || doorwayProp.toBool() == false;
    }
  }
  return false;
}

static char getCoverValue(const TileLayer* layer, const LevelGrid::CaseContent& caseContent, bool hasObstacle)
{
  if (layer)
  {
    Tile* tile = layer->getTile(caseContent.position.x, caseContent.position.y);

    if (tile)
    {
      QVariant coverProp = tile->getProperty("cover");

      if (!coverProp.isNull())
        return static_cast<char>(coverProp.toInt());
    }
  }
  return hasObstacle ? 100 : 0;
}

bool LevelGrid::CaseContent::isBlocked() const
{
  if (!occupied)
  {
    for (const auto* zone : zones)
    {
      if (zone->getAccessBlocked())
        return true;
    }
    return false;
  }
  return true;
}

bool LevelGrid::CaseContent::isLinkedTo(QPoint position) const
{
  auto iterator = std::find_if(connections.begin(), connections.end(), [this, position](const CaseConnection* connection) -> bool
  {
    auto* target = connection->getTargetFor(this);

    return target->position == position && !target->isBlocked();
  });
  return iterator != connections.end();
}

LevelGrid::CaseConnection* LevelGrid::CaseContent::connectionWith(CaseContent* other) const
{
  auto iterator = std::find_if(connections.begin(), connections.end(), [this, other](const CaseConnection* connection) -> bool
  {
    return connection->getTargetFor(this) == other;
  });

  return iterator != connections.end() ? *iterator : nullptr;
}

void LevelGrid::CaseContent::connectWith(CaseContent* other)
{
  if (other)
  {
    CaseConnection* connection = new CaseConnection;

    connection->connect(this, other);
  }
}

void LevelGrid::CaseContent::disconnectFrom(CaseContent* other)
{
  for (auto* connection : connections)
  {
    if (connection->getTargetFor(this) == other)
    {
      connection->disconnect();
      break ;
    }
  }
}

void LevelGrid::CaseContent::clearConnections()
{
  while (connections.size())
    (*connections.begin())->disconnect();
}

void LevelGrid::CaseConnection::connect(CaseContent* a, CaseContent* b)
{
  pair = {a, b};
  a->disconnectFrom(b);
  b->disconnectFrom(a);
  a->connections.push_back(this);
  b->connections.push_back(this);
}

template<typename ARRAY, typename VALUE>
void removeFrom(ARRAY& array, VALUE value)
{
  auto it = std::find(array.begin(), array.end(), value);

  if (it != array.end())
      array.erase(it);
}

void LevelGrid::CaseConnection::disconnect()
{
  removeFrom(pair.first->connections, this);
  removeFrom(pair.second->connections, this);
  delete this;
}

float LevelGrid::CaseConnection::getCost() const
{
  if (doorway && !doorway->property("opened").toBool())
    return 3.f;
  if (pair.first->position.z != pair.second->position.z)
    return 3.f;
  return 1.f;
}

bool LevelGrid::CaseConnection::canGoThrough(CharacterMovement* character)
{
  if (doorway)
    return doorway->canGoThrough(reinterpret_cast<Character*>(character));
  return true;
}

bool LevelGrid::CaseConnection::goThrough(CharacterMovement* character)
{
  if (doorway)
    return doorway->onGoThrough(reinterpret_cast<Character*>(character));
  return true;
}

struct PrepareCaseFunctor
{
  TileMap*   tilemap;
  TileLayer* ground;
  TileLayer* blocks;
  TileLayer* wallsV;
  TileLayer* wallsH;

  void run(int x, int y, LevelGrid::CaseContent& gridCase)
  {
    obstacle(x, y, gridCase);
    cover(gridCase);
  }

private:
  void obstacle(int x, int y, LevelGrid::CaseContent& gridCase)
  {
    gridCase.occupied = gridCase.block = isWall(blocks, x, y);
    gridCase.hwall    = isWall(wallsH, x, y);
    gridCase.vwall    = isWall(wallsV, x, y);
    gridCase.position = Point{x, y, tilemap->getFloor()};
  }

  void cover(LevelGrid::CaseContent& gridCase)
  {
    if (gridCase.block)
      gridCase.cover  = getCoverValue(blocks, gridCase, gridCase.block);
    else
      gridCase.cover  = getCoverValue(ground, gridCase, gridCase.block);
    if (gridCase.hwall)
      gridCase.hcover = getCoverValue(wallsH, gridCase, gridCase.hwall);
    if (gridCase.vwall)
      gridCase.vcover = getCoverValue(wallsV, gridCase, gridCase.vwall);
  }
};

LevelGrid::LevelGrid(QObject *parent) : QObject(parent)
{
}

LevelGrid::~LevelGrid()
{
  for (auto& caseContent : grid)
    caseContent.clearConnections();
}

void LevelGrid::initializeGrid(TileMap* tilemap_)
{
  PrepareCaseFunctor functor;

  tilemap = tilemap_;
  functor.tilemap = tilemap_;
  functor.ground  = tilemap->getLayer("ground");
  functor.blocks  = tilemap->getLayer("blocks");
  functor.wallsV  = tilemap->getLayer("walls-v");
  functor.wallsH  = tilemap->getLayer("walls-h");
  size = tilemap->getSize();
  grid.resize(size.width() * size.height());
  eachCase(std::bind(&PrepareCaseFunctor::run, &functor, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  initializePathfinding();
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

static bool hasTile(const TileLayer& ground, const LevelGrid::CaseContent& caseContent)
{
  return ground.getTile(caseContent.position.x, caseContent.position.y);
}

static bool isCaseAvailable(const TileLayer* ground, const LevelGrid::CaseContent* caseContent)
{
  return caseContent && !caseContent->occupied && hasTile(*ground, *caseContent);
}

void LevelGrid::initializePathfinding()
{
  auto* ground     = tilemap->getLayer("ground");
  auto isAvailable = std::bind(isCaseAvailable, ground, std::placeholders::_1);

  if (!ground)
    return ;
  for (auto& gridCase : grid)
  {
    bool hasLeft  = gridCase.position.x - 1 >= 0;
    bool hasRight = gridCase.position.x + 1 < size.width();
    bool hasUp    = gridCase.position.y - 1 >= 0;
    bool hasDown  = gridCase.position.y + 1 < size.height();

    auto* upLeft    = hasLeft && hasUp    ? getGridCase(gridCase.position.x - 1, gridCase.position.y - 1) : nullptr;
    auto* up        = hasUp               ? getGridCase(gridCase.position.x,     gridCase.position.y - 1) : nullptr;
    auto* upRight   = hasRight && hasUp   ? getGridCase(gridCase.position.x + 1, gridCase.position.y - 1) : nullptr;
    auto* left      = hasLeft             ? getGridCase(gridCase.position.x - 1, gridCase.position.y)     : nullptr;
    auto* right     = hasRight            ? getGridCase(gridCase.position.x + 1, gridCase.position.y)     : nullptr;
    auto* downLeft  = hasDown && hasLeft  ? getGridCase(gridCase.position.x - 1, gridCase.position.y + 1) : nullptr;
    auto* down      = hasDown             ? getGridCase(gridCase.position.x,     gridCase.position.y + 1) : nullptr;
    auto* downRight = hasDown && hasRight ? getGridCase(gridCase.position.x + 1, gridCase.position.y + 1) : nullptr;

    bool upWalled        = up && up->hwall;
    bool upLeftWalled    = (upLeft  && (upLeft->hwall || upLeft->vwall)) || (left && left->vwall) || upWalled;
    bool upRightWalled   = (upRight && upRight->hwall) || (gridCase.vwall) || upWalled || (up && up->vwall);
    bool downWalled      = gridCase.hwall;
    bool downLeftWalled  = gridCase.hwall || (left  && (left->hwall || left->vwall)) || (downLeft && downLeft->vwall);
    bool downRightWalled = gridCase.hwall || gridCase.vwall || (right && right->hwall) || (down && down->vwall);
    bool leftWalled      = left && left->vwall;
    bool rightWalled     = gridCase.vwall;

    gridCase.position.z = tilemap->getFloor();
    gridCase.clearConnections();
    if (!upLeftWalled    && isAvailable(upLeft) && !(!isAvailable(left) && !isAvailable(up)))
      gridCase.connectWith(upLeft);
    if (!upWalled        && isAvailable(up))
      gridCase.connectWith(up);
    if (!upRightWalled   && isAvailable(upRight) && !(!isAvailable(up) && !isAvailable(right)))
      gridCase.connectWith(upRight);
    if (!leftWalled      && isAvailable(left))
      gridCase.connectWith(left);
    if (!rightWalled     && isAvailable(right))
      gridCase.connectWith(right);
    if (!downLeftWalled  && isAvailable(downLeft) && !(!isAvailable(left) && !isAvailable(down)))
      gridCase.connectWith(downLeft);
    if (!downWalled      && isAvailable(down))
      gridCase.connectWith(down);
    if (!downRightWalled && isAvailable(downRight) && !(!isAvailable(down) && !isAvailable(right)))
      gridCase.connectWith(downRight);
  }
}

bool LevelGrid::findPath(Point from, Point to, QList<Point>& path, CharacterMovement* character)
{
  return findPath(from, QVector<Point>{to}, path, character);
}

bool LevelGrid::findPath(Point from, const QVector<Point> &to, QList<Point> &path, CharacterMovement *character, bool quickMode)
{
  typedef AstarPathfinding<LevelGrid::CaseContent> Pathfinder;
  Pathfinder        astar(character);
  unsigned short    iterationCount = 0;
  Pathfinder::State state;
  CaseContent*      fromCase = getGridCase(from.x, from.y, from.z);
  std::list<CaseContent> toCases;

  for (const Point& candidate : to)
  {
    CaseContent* toCase = getGridCase(candidate.x, candidate.y, candidate.z);

    if (toCase)
      toCases.push_back(*toCase);
  }
  if (fromCase && toCases.size() > 0)
  {
    bool fromOccupiedBackup = fromCase->occupied;

    fromCase->occupied = false;
    path.clear();
    astar.SetStartAndGoalStates(*fromCase, toCases);
    if (quickMode)
      astar.AcceptAnyCandidate();
    while ((state = astar.SearchStep()) == Pathfinder::Searching && ++iterationCount < 2500);
    if (iterationCount >= 2500)
      qDebug() << "-> Pathfinding failed due too many iterations" << character->getPath() << static_cast<QPoint>(from);
    auto* solution = astar.GetBestSolution();
    if (solution)
    {
      for (auto& gridCase : *solution)
        path << gridCase.position;
      path.pop_front(); // first case is the starting point
      fromCase->occupied = fromOccupiedBackup;
      return true;
    }
    fromCase->occupied = fromOccupiedBackup;
  }
  else
    qDebug() << "Pathfinding: invalid coordinates" << from << to;
  return false;
}

std::list<LevelGrid::CaseContent*> LevelGrid::CaseContent::GetSuccessors(const CaseContent* parent, Actor* actor) const
{
  std::list<LevelGrid::CaseContent*> results;

  for (auto* connection : connections)
  {
    CaseContent* node = connection->getTargetFor(this);

    if ((!parent || node->position != parent->position) && !node->isBlocked() && connection->canGoThrough(actor))
      results.push_back(node);
  }
  return results;
}

float LevelGrid::CaseContent::GoalDistanceEstimate(const CaseContent& other) const
{
  int distX = position.x - other.position.x;
  int distY = position.y - other.position.y;
  int distFloor = std::abs(position.z - other.position.x);

  return std::sqrt(static_cast<float>(distX * distX + distY * distY)) + static_cast<float>(distFloor * 10);
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
    return gridCase->occupant;
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

void LevelGrid::triggerZone(CharacterMovement* character, int x, int y)
{
  static const QVector<TileZone*> emptyZoneList;
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
