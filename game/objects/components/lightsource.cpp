#include "lightsource.h"
#include "tilemap/tilelayer.h"
#include "game.h"

LightSourceComponent::LightSourceComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &LightSourceComponent::lightRadiusChanged, this, &LightSourceComponent::reloadLightzone);
  connect(this, &LightSourceComponent::lightTypeChanged,   this, &LightSourceComponent::refreshLightzone);
  connect(this, &LightSourceComponent::positionChanged,    this, &LightSourceComponent::refreshLightzone);
}

void LightSourceComponent::load(const QJsonObject& data)
{
  lightRadius = data["lightRadius"].toInt(0);
  lightType = data["lightType"].toInt(0);
  ParentType::load(data);
  emit lightRadiusChanged();
  emit lightTypeChanged();
}

void LightSourceComponent::save(QJsonObject& data) const
{
  if (lightRadius > 0) {
    data["lightRadius"] = lightRadius;
    data["lightType"] = lightType;
  }
  ParentType::save(data);
}

void LightSourceComponent::reloadLightzone()
{
  if (lightRadius == 0 && lightZone)
  {
    emit lightZoneRemoved(lightZone);
    lightZone->deleteLater();
    lightZone = nullptr;
  }
  else if (lightRadius > 0)
  {
    auto* level = Game::get()->getLevel();
    auto* tilemap = level ? level->getTileMap() : nullptr;

    if (!lightZone && tilemap)
    {
      lightZone = new TileLayer(this);
      lightZone->initialize(tilemap->getSize());
      lightZone->setProperty("offset", QPoint(0, 0));
      emit lightZoneAdded(lightZone);
    }
    refreshLightzone();
  }
}

void LightSourceComponent::refreshLightzone()
{
  auto* level = Game::get()->getLevel();

  if (level && lightZone)
  {
    auto*  tilemap = level->getTileMap();
    auto*  tileset = tilemap->getTileset("lights");
    QPoint position = getPosition();
    QRect  zone(position.x() - lightRadius, position.y() - lightRadius, lightRadius * 2 + 1, lightRadius * 2 + 1);

    int bottomTid      = tileset->getFirstGid() + 5;
    int bottomLeftTid  = tileset->getFirstGid() + 8;
    int middleTid      = tileset->getFirstGid() + 4;
    int bottomRightTid = tileset->getFirstGid() + 2;
    int rightTid       = tileset->getFirstGid() + 1;
    int topRightTid    = tileset->getFirstGid() + 0;
    int topTid         = tileset->getFirstGid() + 3;
    int leftTid        = tileset->getFirstGid() + 7;
    int topLeftTid     = tileset->getFirstGid() + 6;

    lightZone->clear();
    lightZone->fill(QRect(zone.x() + 1, zone.y() + 1, zone.width() - 2, zone.height() - 2), tileset, middleTid);
    lightZone->setTileIdAt(zone.x(), zone.y(), tileset, topRightTid);
    lightZone->fill(QRect(zone.x() + 1, zone.y(), zone.width() - 2, 1), tileset, rightTid);
    lightZone->setTileIdAt(zone.right(), zone.y(), tileset, bottomRightTid);
    lightZone->fill(QRect(zone.x(), zone.y() + 1, 1, zone.height() - 2), tileset, topTid);
    lightZone->setTileIdAt(zone.x(), zone.bottom(), tileset, topLeftTid);
    lightZone->fill(QRect(zone.x() + 1, zone.y() + zone.height() - 1, zone.width() - 2, 1), tileset, leftTid);
    lightZone->setTileIdAt(zone.right(), zone.bottom(), tileset, bottomLeftTid);
    lightZone->fill(QRect(zone.x() + zone.width() - 1, zone.y() + 1, 1, zone.height() - 2), tileset, bottomTid);
    qDebug() << "Light zone:" << zone << "at" << position;
  }
}
