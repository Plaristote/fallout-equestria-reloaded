#include "controlzone.h"
#include "tilemap/tilezone.h"
#include "game.h"
#include <QJsonArray>
#include <QDebug>

ControlZoneComponent::ControlZoneComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &ControlZoneComponent::controlZoneAdded,   this, &ControlZoneComponent::listenControlZone);
  connect(this, &ControlZoneComponent::controlZoneAdded,   this, &ControlZoneComponent::controlZoneChanged);
  connect(this, &ControlZoneComponent::controlZoneRemoved, this, &ControlZoneComponent::stopListeningControlZone);
  connect(this, &ControlZoneComponent::controlZoneRemoved, this, &ControlZoneComponent::controlZoneChanged);
  connect(this, &ControlZoneComponent::zoneBlockedChanged, this, &ControlZoneComponent::updateZoneBlock);
  connect(this, &ControlZoneComponent::zoneBlockedChanged, this, &ControlZoneComponent::updateCover);
  connect(this, &GridObjectComponent::coverChanged,        this, &ControlZoneComponent::updateCover);
}

void ControlZoneComponent::initializeZone()
{
  controlZone = new TileZone(this);
  if (metaObject()->inherits(DynamicObject().metaObject()))
    controlZone->setOwner(reinterpret_cast<DynamicObject*>(this));
  onPositionChanged();
  connect(this, &ControlZoneComponent::positionChanged, this, &ControlZoneComponent::onPositionChanged);
  connect(this, &ControlZoneComponent::floorChanged,    this, &ControlZoneComponent::onPositionChanged);
}

TileZone* ControlZoneComponent::addControlZone()
{
  if (controlZone == nullptr)
  {
    initializeZone();
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
    disconnect(this, &ControlZoneComponent::positionChanged, this, &ControlZoneComponent::onPositionChanged);
    disconnect(this, &ControlZoneComponent::floorChanged,    this, &ControlZoneComponent::onPositionChanged);
    emit controlZoneRemoved(backup);
    backup->deleteLater();
  }
}

QJSValue ControlZoneComponent::getControlZoneOccupants()
{
  auto* game  = Game::get();
  auto* level = game->getLevel();

  if (level)
    return level->getZoneOccupants(controlZone);
  return game->getScriptEngine().newArray();
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
  scriptCall("onZoneEntered", QJSValueList() << object->asJSValue() << Game::get()->getScriptEngine().newQObject(zone));
}

void ControlZoneComponent::onZoneExited(DynamicObject* object, TileZone* zone)
{
  scriptCall("onZoneExited", QJSValueList() << object->asJSValue() << Game::get()->getScriptEngine().newQObject(zone));
}

void ControlZoneComponent::onPositionChanged()
{
  controlZone->setOffset(getPosition(), floor);
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

void ControlZoneComponent::updateCover()
{
  if (controlZone && Game::get()->getLevel())
  {
    LevelGrid* grid = Game::get()->getLevel()->getFloorGrid(getCurrentFloor());

    for (QPoint position : controlZone->getPositions())
    {
      auto* gridCase = grid->getGridCase(position + controlZone->getOffset());

      if (gridCase)
        gridCase->cover = zoneBlocked ? static_cast<char>(getCoverValue()) : 0;
    }
  }
}

void ControlZoneComponent::load(const QJsonObject& data)
{
  ParentType::load(data);
  if (data["zone"].isArray())
  {
    if (!controlZone)
      initializeZone();
    zoneBlocked = data["zoneBlocked"].toBool();
    for (const QJsonValue posValue : data["zone"].toArray())
    {
      QJsonArray posArray(posValue.toArray());

      controlZone->addRelativePosition(QPoint(posArray[0].toInt(), posArray[1].toInt()));
    }
    emit zoneBlockedChanged();
    emit controlZoneAdded(controlZone);
  }
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
