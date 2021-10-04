#include "grid.h"
#include "game/characters/actionqueue.h"
#include "tilemap/tilemap.h"
#include "game.h"
#include "game/dices.hpp"
#include <cmath>

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
      floors.push_back(floorGrid);
    }
    emit floorChanged();
    ParentType::load(data);
  }
  else
    throw std::runtime_error("Could not load tilemap");
}

void GridComponent::onRoofVisibilityChanged(TileLayer* layer)
{
  for (DynamicObject* object : qAsConst(objects))
  {
    QPoint position = object->getPosition();

    if (layer->isInside(position.x(), position.y()))
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
  ParentType::unregisterDynamicObject(object);
}

void GridComponent::addCharacterObserver(Character* character, QMetaObject::Connection observer)
{
  characterObservers[character].push_back(observer);
}

DynamicObject* GridComponent::getOccupantAt(QPoint position)
{
  return grid->getOccupant(position.x(), position.y());
}

void GridComponent::onCharacterDied(Character*)
{
}

void GridComponent::setCharacterPosition(Character* character, int x, int y, unsigned char objectFloor)
{
  character->rcurrentPath().clear();
  setObjectPosition(character, x, y, objectFloor);
}

bool GridComponent::moveCharacterToZone(Character* character, const QString& zoneName, unsigned char objectFloor)
{
  if (objectFloor >= getFloors().size())
    objectFloor = this->floor;
  return moveCharacterToZone(character, getFloorGrid(objectFloor)->getTilemap()->getZone(zoneName));
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
        setCharacterPosition(character, candidate.x(), candidate.y(), zone->getFloor());
        return true;
      }
    }
  }
  else
    qDebug() << "GridComponent::moveCharacterToZone: invalid parameters" << character << zone;
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
}

void GridComponent::setRenderObjectPosition(DynamicObject* object, int x, int y)
{
  LevelGrid *grid = getFloorGrid(object->getCurrentFloor());
  QPoint renderPosition = getRenderPositionForTile(x, y);

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
    QRect  rect     = object->getClippedRect();

    if (!object->isFloating())
    {
      const auto extraHeight = rect.height() - tileSize.height();

      return QPoint(
        offset.x() + (tileSize.width() / 2 - rect.width() / 2),
        offset.y() - extraHeight
      );
    }
    return offset;
  }
  return QPoint(0, 0);
}

TileLayer* GridComponent::getRoofFor(DynamicObject* object) const
{
  QPoint position = object->getPosition();

  for (TileLayer* layer : getTileMap()->getRoofs())
  {
    Tile* tile = layer->getTile(position.x(), position.y());

    if (tile)
      return layer;
  }
  return nullptr;
}

QJSValue GridComponent::getDynamicObjectsAt(int x, int y) const
{
  auto& scriptEngine = Game::get()->getScriptEngine();
  QJSValue result = scriptEngine.newArray();
  QJSValue push = result.property("push");
  QPoint   position(x, y);
  auto     objectList = findDynamicObjects(
    [position](DynamicObject& object) { return object.getPosition() == position; }
  );

  for (DynamicObject* object : objectList)
    push.callWithInstance(result, QJSValueList() << object->asJSValue());
  return result;
}

QPoint GridComponent::getRenderPositionForTile(int x, int y)
{
  auto* layer = getTileMap()->getLayer("ground");
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

bool GridComponent::isRenderedBefore(const DynamicObject* a, const DynamicObject* b)
{
  QPoint posA = a->getPosition();
  QPoint posB = b->getPosition();

  if (posA.x() == posB.x())
    return posA.y() > posB.y();
  return posA.x() > posB.x();
}

