#include "controlzone.h"
#include "tilemap/tilezone.h"
#include "game.h"
#include <QJsonArray>

ControlZoneComponent::ControlZoneComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &ControlZoneComponent::controlZoneAdded,   this, &ControlZoneComponent::listenControlZone);
  connect(this, &ControlZoneComponent::controlZoneAdded,   this, &ControlZoneComponent::controlZoneChanged);
  connect(this, &ControlZoneComponent::controlZoneRemoved, this, &ControlZoneComponent::stopListeningControlZone);
  connect(this, &ControlZoneComponent::controlZoneRemoved, this, &ControlZoneComponent::controlZoneChanged);
  connect(this, &ControlZoneComponent::zoneBlockedChanged, this, &ControlZoneComponent::updateZoneBlock);
}

TileZone* ControlZoneComponent::addControlZone()
{
  if (controlZone == nullptr)
  {
    controlZone = new TileZone(this);
    emit controlZoneAdded(controlZone);
  }
  return controlZone;
}

void ControlZoneComponent::removeControlZone()
{
  if (controlZone != nullptr)
  {
    auto* backup = controlZone;

    controlZone = nullptr;
    emit controlZoneRemoved(backup);
    backup->deleteLater();
  }
}

void ControlZoneComponent::listenControlZone(TileZone* zone)
{
  connect(zone, &TileZone::enteredZone, this, &ControlZoneComponent::onZoneEntered);
  connect(zone, &TileZone::exitedZone, this, &ControlZoneComponent::onZoneExited);
}

void ControlZoneComponent::stopListeningControlZone(TileZone* zone)
{
  disconnect(zone, &TileZone::enteredZone, this, &ControlZoneComponent::onZoneEntered);
  disconnect(zone, &TileZone::exitedZone, this, &ControlZoneComponent::onZoneExited);
}

void ControlZoneComponent::onZoneEntered(DynamicObject* object, TileZone* zone)
{
  if (script && script->hasMethod("onZoneEntered"))
    script->call("onZoneEntered", QJSValueList() << object->asJSValue() << Game::get()->getScriptEngine().newQObject(zone));
}

void ControlZoneComponent::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (script && script->hasMethod("onZoneExited"))
    script->call("onZoneExited", QJSValueList() << object->asJSValue() << Game::get()->getScriptEngine().newQObject(zone));
}

void ControlZoneComponent::toggleZoneBlocked(bool value)
{
   if (zoneBlocked != value)
   {
     zoneBlocked = value;
     emit zoneBlockedChanged();
   }
}

void ControlZoneComponent::updateZoneBlock()
{
  if (controlZone)
    controlZone->setAccessBlocked(zoneBlocked);
}

void ControlZoneComponent::load(const QJsonObject& data)
{
  if (data["zone"].isArray())
  {
    controlZone = controlZone ? controlZone : new TileZone(this);
    zoneBlocked = data["zoneBlocked"].toBool();
    for (QJsonValue posValue : data["zone"].toArray())
    {
      QJsonArray posArray(posValue.toArray());

      controlZone->addPosition(QPoint(posArray[0].toInt(), posArray[1].toInt()));
    }
    emit zoneBlockedChanged();
    emit controlZoneAdded(controlZone);
  }
  ParentType::load(data);
}

void ControlZoneComponent::save(QJsonObject& data) const
{
  if (controlZone)
  {
    QJsonArray zoneArray;

    for (QPoint position : controlZone->getPositions())
    {
      QJsonArray posArray;

      posArray << position.x() << position.y();
      zoneArray << posArray;
    }
    data["zone"] = zoneArray;
    data["zoneBlocked"] = zoneBlocked;
  }
  ParentType::save(data);
}
