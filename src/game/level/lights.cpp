#include "lights.h"
#include "tilemap/tilemap.h"

LightingComponent::LightingComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &GridComponent::floorChanged, this, &LightingComponent::onFloorChanged);
}

bool LightingComponent::isLit(QPoint position) const
{
  return m_litTiles.contains(pack(position.x(), position.y()));
}

void LightingComponent::onFloorChanged()
{
  resubscribeToLights();
}

void LightingComponent::onLightsChanged()
{
  resubscribeToLights();
}

void LightingComponent::onLightVisibilityChanged()
{
  rebuildLitTiles();
  emit activeLightsChanged();
}

void LightingComponent::onLightTilesChanged()
{
  auto* layer = qobject_cast<TileLayer*>(sender());

  rebuildLitTiles();
  if (layer)
    emit lightLayerTilesChanged(layer);
}

void LightingComponent::resubscribeToLights()
{
  TileMap* tilemap = getTileMap();

  for (const QMetaObject::Connection &c : m_lightConnections)
    disconnect(c);
  m_lightConnections.clear();
  if (tilemap)
  {
    m_lightConnections << connect(tilemap, &TileMap::lightsChanged, this, &LightingComponent::onLightsChanged);
    for (TileLayer *light : tilemap->getLights())
    {
      if (!light)
        continue;
      m_lightConnections << connect(light, &TileLayer::visibleChanged, this, &LightingComponent::onLightVisibilityChanged);
      m_lightConnections << connect(light, &TileLayer::tilesChanged, this, &LightingComponent::onLightTilesChanged);
    }
  }
  rebuildLitTiles();
  emit activeLightsChanged();
}

void LightingComponent::rebuildLitTiles()
{
  TileMap*    tilemap = getTileMap();
  const QSize mapSize = tilemap->getSize();

  m_litTiles.clear();
  m_activeLights.clear();
  for (TileLayer *light : tilemap->getLights())
  {
    if (light->isVisible())
    {
      m_activeLights.push_back(light);
      for (int y = 0; y < mapSize.height(); ++y)
      {
        for (int x = 0; x < mapSize.width(); ++x)
        {
          if (light->getTile(x, y))
            m_litTiles.insert(pack(x, y));
        }
      }
    }
  }
}
