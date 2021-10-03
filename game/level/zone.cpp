#include "zone.h"
#include "tilemap/tilezone.h"
#include "tilemap/tilemap.h"
#include "game/objects/doorway.h"
#include "game.h"

ZoneComponent::ZoneComponent(QObject* parent) : ParentType(parent)
{

}

void ZoneComponent::load(const QJsonObject& data)
{
  ParentType::load(data);
  for (auto* floor : getFloors())
  {
    for (auto* zone : floor->getTilemap()->getZones())
      registerZone(zone);
  }
}

void ZoneComponent::registerZoneController(ControlZoneComponent* object)
{
  connect(object, &DynamicObject::controlZoneAdded,   this, &ZoneComponent::registerZone);
  connect(object, &DynamicObject::controlZoneRemoved, this, &ZoneComponent::unregisterZone);
  if (object->getControlZone())
    registerZone(object->getControlZone());
}

void ZoneComponent::unregisterZoneController(ControlZoneComponent* object)
{
  disconnect(object, &ControlZoneComponent::controlZoneAdded,   this, &ZoneComponent::registerZone);
  disconnect(object, &ControlZoneComponent::controlZoneRemoved, this, &ZoneComponent::unregisterZone);
  if (object->getControlZone())
    unregisterZone(object->getControlZone());
}

void ZoneComponent::registerDynamicObject(DynamicObject* object)
{
  registerZoneController(object);
  if (object->isCharacter())
    dynamic_cast<CharacterMovement*>(object)->setCurrentZones(getGrid()->getZonesAt(object->getPosition()));
  else if (object->getObjectType() == "Doorway")
    dynamic_cast<Doorway*>(object)->updateTileConnections();
  ParentType::registerDynamicObject(object);
}

void ZoneComponent::unregisterDynamicObject(DynamicObject* object)
{
  unregisterZoneController(object);
  if (object->isCharacter())
    dynamic_cast<CharacterMovement*>(object)->clearCurrentZones();
  else if (object->getObjectType() == "Doorway")
    dynamic_cast<Doorway*>(object)->removeTileConnections();
  ParentType::unregisterDynamicObject(object);
}

void ZoneComponent::registerZone(TileZone* zone)
{
  LevelGrid* floor = getFloors().at(0);

  floor->getTilemap()->addTileZone(zone);
  floor->registerZone(zone);
  connect(zone, &TileZone::enteredZone, this, &ZoneComponent::onZoneEntered, Qt::QueuedConnection);
  connect(zone, &TileZone::exitedZone,  this, &ZoneComponent::onZoneExited,  Qt::QueuedConnection);
}

void ZoneComponent::unregisterZone(TileZone* zone)
{
  LevelGrid* floor = getFloors().at(0);

  disconnect(zone, &TileZone::enteredZone, this, &ZoneComponent::onZoneEntered);
  disconnect(zone, &TileZone::exitedZone,  this, &ZoneComponent::onZoneExited);
  floor->unregisterZone(zone);
  floor->getTilemap()->removeTileZone(zone);
  for (DynamicObject* object : qAsConst(objects))
  {
    if (object->isCharacter())
    {
      CharacterMovement* character = reinterpret_cast<CharacterMovement*>(object);

      if (character->getCurrentZones().contains(zone))
      {
        QVector<TileZone*> zones(character->getCurrentZones());

        zones.removeAll(zone);
        character->setCurrentZones(zones);
      }
    }
  }
}

void ZoneComponent::onZoneEntered(DynamicObject* object, TileZone* zone)
{
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    if (!character->isInZone(zone))
    {
      character->onZoneEntered(zone);
      if (script && script->hasMethod("onZoneEntered"))
        script->call("onZoneEntered", QJSValueList() << zone->getName() << character->asJSValue());
      if (zone->getType() == "exit" && object == getPlayer())
        emit exitZoneEntered(zone);
    }
  }
}

void ZoneComponent::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (object->isCharacter())
  {
    reinterpret_cast<Character*>(object)->onZoneExited(zone);
    if (script && script->hasMethod("onZoneExited"))
      script->call("onZoneExited", QJSValueList() << zone->getName() << object->asJSValue());
  }
}


QJSValue ZoneComponent::getZoneOccupants(TileZone* zone)
{
  auto*    game = Game::get();
  auto&    scriptEngine = game->getScriptEngine();
  QJSValue result = scriptEngine.newArray();

  if (zone)
  {
    for (QPoint position : zone->getPositions())
    {
      QJSValue concat, objectArray;

      position   += zone->getOffset();
      concat      = result.property("concat");
      objectArray = game->getLevel()->getDynamicObjectsAt(position.x(), position.y());
      result      = concat.callWithInstance(result, QJSValueList() << objectArray);
    }
  }
  return result;
}
