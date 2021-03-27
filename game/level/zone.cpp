#include "zone.h"
#include "tilemap/tilezone.h"

ZoneComponent::ZoneComponent(QObject* parent) : GridComponent(parent)
{

}

void ZoneComponent::registerDynamicObject(DynamicObject* object)
{
  connect(object, &DynamicObject::controlZoneAdded,   this, &ZoneComponent::registerZone);
  connect(object, &DynamicObject::controlZoneRemoved, this, &ZoneComponent::unregisterZone);
  if (object->getControlZone())
    registerZone(object->getControlZone());
  ParentType::registerDynamicObject(object);
}

void ZoneComponent::unregisterDynamicObject(DynamicObject* object)
{
  disconnect(object, &DynamicObject::controlZoneAdded,   this, &ZoneComponent::registerZone);
  disconnect(object, &DynamicObject::controlZoneRemoved, this, &ZoneComponent::unregisterZone);
  if (object->getControlZone())
    unregisterZone(object->getControlZone());
  ParentType::unregisterDynamicObject(object);
}

void ZoneComponent::registerZone(TileZone* zone)
{
  grid->registerZone(zone);
  connect(zone, &TileZone::enteredZone, this, &ZoneComponent::onZoneEntered, Qt::QueuedConnection);
  connect(zone, &TileZone::exitedZone,  this, &ZoneComponent::onZoneExited,  Qt::QueuedConnection);
}

void ZoneComponent::unregisterZone(TileZone* zone)
{
  disconnect(zone, &TileZone::enteredZone, this, &ZoneComponent::onZoneEntered);
  disconnect(zone, &TileZone::exitedZone,  this, &ZoneComponent::onZoneExited);
  grid->unregisterZone(zone);
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
    }
  }
}

void ZoneComponent::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (object->isCharacter())
    reinterpret_cast<Character*>(object)->onZoneExited(zone);
}
