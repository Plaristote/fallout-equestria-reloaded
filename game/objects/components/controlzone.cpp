#include "controlzone.h"
#include "tilemap/tilezone.h"
#include <QJsonArray>

ControlZoneComponent::ControlZoneComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &ControlZoneComponent::controlZoneAdded,   this, &ControlZoneComponent::controlZoneChanged);
  connect(this, &ControlZoneComponent::controlZoneRemoved, this, &ControlZoneComponent::controlZoneChanged);
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
    emit controlZoneRemoved(controlZone);
    delete controlZone;
  }
  controlZone = nullptr;
}

void ControlZoneComponent::load(const QJsonObject& data)
{
  if (data["zone"].isArray())
  {
    controlZone = controlZone ? controlZone : new TileZone(this);
    for (QJsonValue posValue : data["zone"].toArray())
    {
      QJsonArray posArray(posValue.toArray());

      controlZone->addPosition(QPoint(posArray[0].toInt(), posArray[1].toInt()));
      controlZone->setAccessBlocked(true);
    }
    emit controlZoneChanged();
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
  }
  ParentType::save(data);
}
