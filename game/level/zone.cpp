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
  LevelGrid* floor = getFloorGrid(zone->getFloor());

  if (floor)
  {
    floor->getTilemap()->addTileZone(zone);
    floor->registerZone(zone);
    connect(zone, &TileZone::enteredZone,  this, &ZoneComponent::onZoneEntered);
    connect(zone, &TileZone::exitedZone,   this, &ZoneComponent::onZoneExited);
    connect(zone, &TileZone::floorChanged, this, &ZoneComponent::onZoneChangedFloor);
  }
}

void ZoneComponent::unregisterZone(TileZone* zone)
{
  LevelGrid* floor = getFloorGrid(zone->getFloor());

  if (floor)
  {
    disconnect(zone, &TileZone::enteredZone,  this, &ZoneComponent::onZoneEntered);
    disconnect(zone, &TileZone::exitedZone,   this, &ZoneComponent::onZoneExited);
    disconnect(zone, &TileZone::floorChanged, this, &ZoneComponent::onZoneChangedFloor);
    floor->unregisterZone(zone);
    floor->getTilemap()->removeTileZone(zone);
    for (DynamicObject* object : allDynamicObjects())
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
}

TileZone * ZoneComponent::getZoneFromName(const QString& name) const
{
  for (unsigned char i = 0 ; i < getFloorCount() ; ++i)
  {
    LevelGrid* floor = getFloorGrid(i);

    for (TileZone* zone : floor->getZones())
    {
      if (zone->getName() == name)
        return zone;
    }
  }
  return nullptr;
}

void ZoneComponent::onZoneChangedFloor(TileZone* zone)
{
  qDebug() << "onZoneChangedFloor" << zone;
  for (unsigned char i = 0 ; i < getFloorCount() ; ++i)
  {
    LevelGrid* floor = getFloorGrid(i);

    if (zone->getFloor() == i)
    {
      floor->getTilemap()->addTileZone(zone);
      floor->registerZone(zone);
    }
    else
    {
      floor->unregisterZone(zone);
      floor->getTilemap()->removeTileZone(zone);
    }
  }
}

void ZoneComponent::onZoneEntered(DynamicObject* object, TileZone* zone)
{
  QJSValue result;

  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    if (!character->isInZone(zone))
      character->onZoneEntered(zone);
  }
  result = scriptCall("onZoneEntered", QJSValueList() << zone->getName() << object->asJSValue());
  if (result.isBool() && result.toBool() == true)
    return ;
  if (zone->getType() == "exit" && object == getPlayer())
    emit exitZoneEntered(zone);
}

void ZoneComponent::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (object->isCharacter())
  {
    reinterpret_cast<Character*>(object)->onZoneExited(zone);
    scriptCall("onZoneExited", QJSValueList() << zone->getName() << object->asJSValue());
  }
}

bool ZoneComponent::isInsideZone(const TileZone* zone, const DynamicObject* object) const
{
  return zone && object
      && zone->getFloor() == object->getCurrentFloor()
      && zone->getPositions().indexOf(object->getPosition()) >= 0;
}

QJSValue ZoneComponent::getZoneOccupants(const TileZone* zone) const
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
      objectArray = game->getLevel()->getDynamicObjectsAt(position.x(), position.y(), zone->getFloor());
      result      = concat.callWithInstance(result, QJSValueList() << objectArray);
    }
  }
  return result;
}
