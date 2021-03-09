#include "grid.h"
#include "game/characters/actionqueue.h"
#include "tilemap/tilezone.h"

GridComponent::GridComponent(QObject *parent) : QObject(parent)
{
  grid = new LevelGrid(this);
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
      connect(character, &Sprite::movementFinished, this, [this, character]() { onMovementFinished(character); }),
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

void GridComponent::onCharacterDied(Character* character)
{
  grid->removeObject(character);
}

void GridComponent::onMovementFinished(Character* object)
{
  auto position = object->getPosition();

  grid->triggerZone(object, position.x(), position.y());
  if (object->rcurrentPath().size() > 0)
  {
    object->rcurrentPath().pop_front();
    if (object->getCurrentPath().size() > 0)
    {
      QPoint nextCase = object->getCurrentPath().first();

      if (!startCharacterMoveToTile(object, nextCase.x(), nextCase.y()))
      {
        object->getActionQueue()->reset();
        emit object->getActionQueue()->queueCompleted();
        emit object->pathBlocked(); // Remove ?
      }
    }
    else
    {
      object->onIdle();
      emit object->reachedDestination();
    }
  }
  else
    qDebug() << "!!";
}

bool GridComponent::startCharacterMoveToTile(Character* character, int x, int y)
{
  if (grid->moveObject(character, x, y))
  {
    QPoint renderPosition = getRenderPositionForTile(x, y);

    character->moveTo(x, y, renderPosition);
    return true;
  }
  return false;
}

void GridComponent::setCharacterPosition(Character* character, int x, int y)
{
  character->rcurrentPath().clear();
  setObjectPosition(character, x, y);
}

void GridComponent::setObjectPosition(DynamicObject* object, int x, int y)
{
  grid->moveObject(object, x, y);
  object->setPosition(QPoint(x, y));
  if (!object->isFloating())
  {
    QPoint renderPosition = getRenderPositionForTile(x, y);

    object->setRenderPosition(renderPosition);
  }
}

bool GridComponent::moveTo(Character* character, QPoint targetPosition)
{
  QPoint position = character->getPosition();
  QList<QPoint> path;

  if (grid->findPath(position, targetPosition, path))
  {
    auto& currentPath = character->rcurrentPath();

    while (currentPath.size() > 1)
      currentPath.pop_back();
    for (auto point : qAsConst(path))
      currentPath.append(point);
    if (character->getCurrentPath().size() > 0)
    {
      QPoint nextCase = character->getCurrentPath().first();

      startCharacterMoveToTile(character, nextCase.x(), nextCase.y());
    }
    else
      emit character->reachedDestination();
    return true;
  }
  return false;
}
