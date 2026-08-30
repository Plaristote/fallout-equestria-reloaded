#pragma once
#include "../itemsrenderer.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilemap.h"

class ZonesRenderer : public ItemsRendererInterface
{
public:
  ZonesRenderer(TextureCache*);

  void setTilemap(TileMap* tilemap, int floor) override;
  void operator()(std::vector<RenderItem>& out, qreal) override;

  const QVariantList& visibleZones() const { return m_visibleZones; }
  void setVisibleZones(const QVariantList& value) { m_visibleZones = value; }

private:
  bool isZoneVisible(TileZone* zone) const;
  void renderZone(std::vector<RenderItem>& out, TileZone* zone);

  TileLayer*    m_ground;
  QSGTexture*   m_tilesetTexture;
  QVariantList  m_visibleZones;
};
