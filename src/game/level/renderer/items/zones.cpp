#include "zones.h"

ZonesRenderer::ZonesRenderer(TextureCache* textureCache)
  : ItemsRendererInterface(textureCache)
{
  const QString zoneTilesetPath = ASSETS_PATH + QStringLiteral("tilesets/zones.png");
  m_tilesetTexture = m_textureCache.texture(zoneTilesetPath);
}

void ZonesRenderer::setTilemap(TileMap* tilemap, int floor)
{
  ItemsRendererInterface::setTilemap(tilemap, floor);
  m_ground  = tilemap->getLayer(QStringLiteral("ground"));
}

void ZonesRenderer::operator()(std::vector<RenderItem>& out, qreal)
{
  for (TileZone* zone : m_tilemap->getZones())
  {
    if (isZoneVisible(zone))
      renderZone(out, zone);
  }
}

bool ZonesRenderer::isZoneVisible(TileZone* zone) const
{
  if (zone->isEnabled())
  {
    if (zone->getType() == QStringLiteral("exit"))
      return true;
    for (const QVariant& v : m_visibleZones)
    {
      if (v.value<QObject*>() == zone)
        return true;
    }
  }
  return false;
}

void ZonesRenderer::renderZone(std::vector<RenderItem>& out, TileZone* zone)
{
  const int    count = zone->getPositionCount();
  const QRect  clip  = zone->getClippedRect();
  const QSizeF destSize(clip.width(), clip.height());

  for (int i = 0; i < count; ++i)
  {
    const QPoint position       = zone->getPositionAt(i);
    const Tile*  tile           = m_ground->getTile(position.x(), position.y());
    const QPoint renderPosition = tile ? tile->getRenderPosition() : QPoint(0, 0);
    RenderItem item;

    item.layer                = RenderLayer::Zone;
    item.texture              = m_tilesetTexture;
    item.zKey                 = 0;
    item.destRect             = QRectF(renderPosition + m_worldShift, destSize);
    item.sourceRectPx         = clip;
    item.excludedFromDaylight = true;
    out.push_back(item);
  }
}
