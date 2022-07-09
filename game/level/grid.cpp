#include "grid.h"
#include "game/characters/actionqueue.h"
#include "tilemap/tilemap.h"
#include "game.h"
#include "game/dices.hpp"
#include "game/objects/elevator.h"
#include <cmath>

LevelGrid* getFloorGrid(unsigned char floor)
{
  auto* level = Game::get()->getLevel();

  return level ? level->getFloorGrid(floor) : nullptr;
}

GridComponent::GridComponent(QObject *parent) : ParentType(parent)
{
  grid = new LevelGrid(this);
}

void GridComponent::load(const QJsonObject& data)
{
  auto* tilemap = new TileMap(this);

  if (tilemap->load(data["name"].toString()))
  {
    for (TileLayer* layer : tilemap->getRoofs())
      connect(layer, &TileLayer::visibleChanged, [this, layer]() { onRoofVisibilityChanged(layer); });
    grid->initializeGrid(tilemap);
    floors.push_back(grid);
    for (FloorLayer* layer : tilemap->getFloors())
    {
      LevelGrid* floorGrid = new LevelGrid(this);

      floorGrid->initializeGrid(layer->getTileMap());
      if (!floorGrid->getTilemap()->getLayer("ground"))
        throw std::runtime_error("Missing ground layer in tilemap");
      floors.push_back(floorGrid);
    }
    currentFloor = static_cast<unsigned char>(data["currentFloor"].toInt(0));
    emit floorChanged();
    pathfinding.prepareZoneGrid(floors);
    ParentType::load(data);
  }
  else
    throw std::runtime_error("Could not load tilemap");
}

void GridComponent::save(QJsonObject& data) const
{
  data["currentFloor"] = static_cast<int>(currentFloor);
  ParentType::save(data);
}

void GridComponent::setCurrentFloor(unsigned int value)
{
  if (value != static_cast<unsigned int>(currentFloor))
  {
    if (value >= getFloorCount())
      value = getFloorCount() - 1;
    currentFloor = static_cast<unsigned char>(value);
    emit floorChanged();
  }
}

void GridComponent::onRoofVisibilityChanged(TileLayer* layer)
{
  TileMask* mask = getTileMap()->getMaskLayerFor(layer);

  for (DynamicObject* object : qAsConst(objects))
  {
    QPoint position = object->getPosition();

    if (mask->isInside(position.x(), position.y()))
      object->setVisible(!layer->isVisible());
  }
}

void GridComponent::registerDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    characterObservers.insert(character, {
      connect(character, &Character::died, this, [this, character]() { onCharacterDied(character); })
    });
  }
  else if (object->getObjectType() == "Elevator")
  {
    Elevator* elevator = reinterpret_cast<Elevator*>(object);

    connect(this, &GridComponent::floorChanged, elevator, &Elevator::onLevelFloorChanged);
    elevator->connectCases();
  }
  objectObservers.insert(object, {
    connect(object, &DynamicObject::blocksPathChanged, this, std::bind(&GridComponent::onPathBlockedChanged, this, object))
  });
  ParentType::registerDynamicObject(object);
}

void GridComponent::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    character->rcurrentPath().clear();
    character->onIdle();
    for (auto observer : characterObservers.value(character))
      disconnect(observer);
    characterObservers.remove(character);
  }
  else if (object->getObjectType() == "Elevator")
  {
    Elevator* elevator = reinterpret_cast<Elevator*>(object);

    disconnect(this, &GridComponent::floorChanged, elevator, &Elevator::onLevelFloorChanged);
    elevator->disconnectCases();
  }
  for (auto observer : objectObservers.value(object))
    disconnect(observer);
  setBlockPathBeahviour(object, false);
  ParentType::unregisterDynamicObject(object);
}

void GridComponent::addCharacterObserver(Character* character, QMetaObject::Connection observer)
{
  characterObservers[character].push_back(observer);
}

DynamicObject* GridComponent::getOccupantAt(QPoint casePosition, unsigned char caseFloor)
{
  if (caseFloor >= floors.size())
    caseFloor = currentFloor;
  return getFloorGrid(caseFloor)->getOccupant(casePosition.x(), casePosition.y());
}

void GridComponent::onCharacterDied(Character*)
{
}

void GridComponent::onPathBlockedChanged(DynamicObject* object)
{
  setBlockPathBeahviour(object, object->isBlockingPath());
}

void GridComponent::setBlockPathBeahviour(DynamicObject* object, bool blockPath)
{
  Point point = object->getPoint();
  auto* grid = getFloorGrid(point.z);

  if (grid)
  {
    if (blockPath)
      grid->insertObject(object, point.x, point.y);
    else
      grid->removeObject(object);
  }
}

void GridComponent::setCharacterPosition(Character* character, int x, int y, unsigned char objectFloor)
{
  LevelGrid* grid;

  if (objectFloor == NULL_FLOOR)
    objectFloor = static_cast<unsigned char>(character->getCurrentFloor());
  grid = getFloorGrid(objectFloor);
  if (grid)
  {
    character->rcurrentPath().clear();
    setObjectPosition(character, x, y, objectFloor);
  }
}

TileZone* GridComponent::getDefaultEntryZone() const
{
  for (auto* zone : floors.at(0)->getTilemap()->getZones())
  {
    if (zone->getType() == "entry" && zone->getIsDefault())
      return zone;
  }
  return nullptr;
}

bool GridComponent::moveCharacterToZone(Character* character, const QString& zoneName, unsigned char objectFloor)
{
  TileZone* zone;

  if (objectFloor >= getFloors().size())
    objectFloor = this->floor;
  zone = zoneName == ""
      ? getDefaultEntryZone()
      : getFloorGrid(objectFloor)->getTilemap()->getZone(zoneName);
  return moveCharacterToZone(character, zone);
}

bool GridComponent::moveCharacterToZone(Character* character, TileZone* zone)
{
  if (character && zone)
  {
    auto candidates = zone->getPositions();
    LevelGrid* grid = getFloorGrid(static_cast<unsigned char>(zone->getFloor()));

    while (candidates.size() > 0 && grid)
    {
      const int randomValue = Dices::Throw(static_cast<unsigned int>(candidates.size() - 1));
      QPoint candidate = candidates.takeAt(randomValue) + zone->getOffset();

      if (!grid->isOccupied(candidate.x(), candidate.y()))
      {
        setCharacterPosition(character, candidate.x(), candidate.y(), static_cast<unsigned char>(zone->getFloor()));
        return true;
      }
    }
    qDebug() << "GridComponent::moveCharacterToZone: could not find teleport" << character->getPath() << ": no cases available in zone | candidate count:" << zone->getPositionCount();
  }
  else
    qDebug() << "GridComponent::moveCharacterToZone: invalid parameters" << character << zone;
  return false;
}

bool GridComponent::insertPartyIntoZone(CharacterParty* party, const QString& zoneName)
{
  for (auto* zone : getTileMap()->getZones())
  {
    if (zone->getType() == "entry" && (zone->getName() == zoneName || (zone->getIsDefault() && zoneName == "")))
      return party->insertIntoZone(this, zone);
  }
  return false;
}

QPoint GridComponent::getTilePosition(QPoint tilePosition) const
{
  return getTileMap()->getPointFor(tilePosition.x(), tilePosition.y());
}

void GridComponent::setObjectPosition(DynamicObject* object, int x, int y, unsigned char objectFloor)
{
  if (objectFloor >= getFloors().size())
    objectFloor = static_cast<unsigned char>(object->getCurrentFloor());
  setGridObjectPosition(object, x, y, objectFloor);
  if (!object->isFloating())
    setRenderObjectPosition(object, x, y);
}

void GridComponent::setGridObjectPosition(DynamicObject* object, int x, int y, unsigned char objectFloor)
{
  LevelGrid* grid = getFloorGrid(objectFloor);
  LevelGrid* lastGrid;

  if (object->getCurrentFloor() == objectFloor)
    grid->moveObject(object, x, y);
  else
  {
    lastGrid = getFloorGrid(object->getCurrentFloor());
    lastGrid->removeObject(object);
    grid->insertObject(object, x, y);
  }
  grid->triggerZone(object, x, y);
}

void GridComponent::setRenderObjectPosition(DynamicObject* object, int x, int y)
{
  LevelGrid *grid = getFloorGrid(object->getCurrentFloor());
  QPoint renderPosition = getRenderPositionForTile(x, y, static_cast<unsigned char>(object->getCurrentFloor()));

  if (object->isCharacter())
    renderPosition.ry() -= grid->getTilemap()->getTileSize().height() / 4;
  object->setRenderPosition(renderPosition);
}

QPoint GridComponent::getAdjustedOffsetFor(const DynamicObject* object) const
{
  if (object)
  {
    QSize  tileSize = getTileMap()->getTileSize();
    QPoint offset   = object->getSpritePosition();

    if (!object->isFloating())
    {
      const QRect rect        = object->getClippedRect();
      const auto  extraHeight = rect.height() - tileSize.height();

      return QPoint(
        offset.x() + (tileSize.width() / 2 - rect.width() / 2),
        offset.y() - extraHeight
      );
    }
    return offset;
  }
  return QPoint(0, 0);
}

TileLayer* GridComponent::getRoofFor(const DynamicObject* object) const
{
  QPoint position = object->getPosition();
  LevelGrid* grid = getFloorGrid(object->getCurrentFloor());

  if (grid)
  {
    TileMap* tilemap = grid->getTilemap();

    for (TileLayer* layer : tilemap->getRoofs())
    {
      TileMask* mask = tilemap->getMaskLayerFor(layer);

      if (mask->isInside(position.x(), position.y()))
        return layer;
    }
  }
  return nullptr;
}

QJSValue GridComponent::getDynamicObjectsAt(int x, int y, unsigned int floor_) const
{
  auto&         scriptEngine = Game::get()->getScriptEngine();
  unsigned char objectFloor = static_cast<int>(floor_) < floors.size() ? static_cast<unsigned char>(floor_) : currentFloor;
  QJSValue      result = scriptEngine.newArray();
  QJSValue      push = result.property("push");
  QPoint        position(x, y);
  const auto    objectList = findDynamicObjects(
    [position, objectFloor](DynamicObject& object) { return object.getPosition() == position && object.getCurrentFloor() == objectFloor; }
  );

  for (DynamicObject* object : objectList)
    push.callWithInstance(result, QJSValueList() << object->asJSValue());
  return result;
}

QVector<DynamicObject*> GridComponent::getDynamicObjectsAt(Point position) const
{
  return findDynamicObjects(
    [position](DynamicObject& object) { return object.getPoint() == position; }
  );
}

QPoint GridComponent::getRenderPositionForTile(int x, int y, unsigned char z)
{
  auto* grid  = z != NULL_FLOOR ? getFloorGrid(z) : getGrid();
  auto* layer = grid  ? grid->getTilemap()->getLayer("ground") : nullptr;
  auto* tile  = layer ? layer->getTile(x, y) : nullptr;

  return tile ? tile->getRenderPosition() : QPoint();
}

float GridComponent::getDistance(QPoint pa, QPoint pb) const
{
  auto a = pa.x() - pb.x();
  auto b = pa.y() - pb.y();

  return std::sqrt(static_cast<float>(a * a + b * b));
}

int GridComponent::getVisionQuality(int ax, int ay, int bx, int by) const
{
  return getGrid()->getVisionQuality(ax, ay, bx, by);
}

TileZone* GridComponent::getTileZone(const QString& zoneName) const
{
  TileZone* result = nullptr;
  for (LevelGrid* grid : floors)
  {
    result = grid->getTilemap()->getZone(zoneName);
    if (result) break ;
  }
  return result;
}

bool GridComponent::isRenderedBefore(const DynamicObject* a, const DynamicObject* b)
{
  QPoint posA = a->getPosition();
  QPoint posB = b->getPosition();

  if (posA.x() == posB.x())
    return posA.y() > posB.y();
  return posA.x() > posB.x();
}
