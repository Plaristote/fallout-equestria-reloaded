#include "zone.h"
#include "tilemap/tilezone.h"
#include "tilemap/tilemap.h"
#include "game.h"

ZoneComponent::ZoneComponent(QObject* parent) : GridComponent(parent)
{

}

void ZoneComponent::registerDynamicObject(DynamicObject* object)
{
  connect(object, &DynamicObject::controlZoneAdded,   this, &ZoneComponent::registerZone);
  connect(object, &DynamicObject::controlZoneRemoved, this, &ZoneComponent::unregisterZone);
  if (object->getControlZone())
    registerZone(object->getControlZone());
  if (object->isCharacter())
    dynamic_cast<CharacterMovement*>(object)->setCurrentZones(getGrid()->getZonesAt(object->getPosition()));
  ParentType::registerDynamicObject(object);
}

void ZoneComponent::unregisterDynamicObject(DynamicObject* object)
{
  disconnect(object, &DynamicObject::controlZoneAdded,   this, &ZoneComponent::registerZone);
  disconnect(object, &DynamicObject::controlZoneRemoved, this, &ZoneComponent::unregisterZone);
  if (object->getControlZone())
    unregisterZone(object->getControlZone());
  if (object->isCharacter())
    dynamic_cast<CharacterMovement*>(object)->clearCurrentZones();
  ParentType::unregisterDynamicObject(object);
}

void ZoneComponent::registerZone(TileZone* zone)
{
  tilemap->addTileZone(zone);
  grid->registerZone(zone);
  connect(zone, &TileZone::enteredZone, this, &ZoneComponent::onZoneEntered, Qt::QueuedConnection);
  connect(zone, &TileZone::exitedZone,  this, &ZoneComponent::onZoneExited,  Qt::QueuedConnection);
}

void ZoneComponent::unregisterZone(TileZone* zone)
{
  disconnect(zone, &TileZone::enteredZone, this, &ZoneComponent::onZoneEntered);
  disconnect(zone, &TileZone::exitedZone,  this, &ZoneComponent::onZoneExited);
  grid->unregisterZone(zone);
  tilemap->removeTileZone(zone);
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
      if (zone->getType() == "exit" && object == getPlayer())
        emit exitZoneEntered(zone);
      else if (script && script->hasMethod("onZoneEntered"))
        script->call("onZoneEntered", QJSValueList() << zone->getName() << character->asJSValue());
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
    for (const QPoint& position : zone->getPositions())
    {
      QJSValue concat = result.property("concat");
      QJSValue objectArray = game->getLevel()->getDynamicObjectsAt(position.x(), position.y());

      result = concat.callWithInstance(result, QJSValueList() << objectArray);
    }
  }
  return result;
}
