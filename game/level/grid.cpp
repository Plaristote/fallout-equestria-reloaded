#include "grid.h"
#include "game/characters/actionqueue.h"
#include "tilemap/tilezone.h"
#include "tilemap/tilemap.h"
#include "game.h"

GridComponent::GridComponent(QObject *parent) : LevelBase(parent)
{
  grid = new LevelGrid(this);
}

void GridComponent::load()
{
  for (TileLayer* layer : tilemap->getRoofs())
    connect(layer, &TileLayer::visibleChanged, [this, layer]() { onRoofVisibilityChanged(layer); });
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

void GridComponent::registerZone(TileZone* zone)
{
  grid->registerZone(zone);
  connect(zone, &TileZone::enteredZone, this, &GridComponent::onZoneEntered, Qt::QueuedConnection);
  connect(zone, &TileZone::exitedZone,  this, &GridComponent::onZoneExited,  Qt::QueuedConnection);
}

void GridComponent::unregisterZone(TileZone* zone)
{
  disconnect(zone, &TileZone::enteredZone, this, &GridComponent::onZoneEntered);
  disconnect(zone, &TileZone::exitedZone,  this, &GridComponent::onZoneExited);
  grid->unregisterZone(zone);
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
}

void GridComponent::addCharacterObserver(Character* character, QMetaObject::Connection observer)
{
  characterObservers[character].push_back(observer);
}

DynamicObject* GridComponent::getOccupantAt(QPoint position)
{
  return grid->getOccupant(position.x(), position.y());
}

void GridComponent::onZoneEntered(DynamicObject* object, TileZone* zone)
{
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    if (!character->isInZone(zone))
    {
      character->onZoneEntered(zone);
      if (zone->getType() == "exit" && object == getPlayer())
        emit exitZoneEntered(zone);
    }
  }
}

void GridComponent::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (object->isCharacter())
    reinterpret_cast<Character*>(object)->onZoneExited(zone);
}

void GridComponent::onCharacterDied(Character*)
{
}

void GridComponent::setCharacterPosition(Character* character, int x, int y)
{
  character->rcurrentPath().clear();
  setObjectPosition(character, x, y);
}

void GridComponent::setObjectPosition(DynamicObject* object, int x, int y)
{
  if (object->isBlockingPath())
    grid->moveObject(object, x, y);
  object->setPosition(QPoint(x, y));
  if (!object->isFloating())
  {
    QPoint renderPosition = getRenderPositionForTile(x, y);

    object->setRenderPosition(renderPosition);
  }
}

QPoint GridComponent::getAdjustedOffsetFor(DynamicObject* object) const
{
  if (object)
  {
    QSize  tileSize = tilemap->getTileSize();
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

  for (TileLayer* layer : tilemap->getRoofs())
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
  QPoint position(x, y);

  for (DynamicObject* object : objects)
  {
    if (object->getPosition() == position)
      push.callWithInstance(result, QJSValueList() << object->asJSValue());
  }
  return result;
}
