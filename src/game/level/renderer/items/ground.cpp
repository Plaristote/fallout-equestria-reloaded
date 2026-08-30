#include "ground.h"

void GroundRenderer::setTilemap(TileMap* tilemap, int floor)
{
  QString path;

  ItemsRendererInterface::setTilemap(tilemap, floor);
  path = QStringLiteral("%1floor%2_tilemap.png")
             .arg(m_pathPrefix)
             .arg(floor);
  m_ground     = tilemap->getLayer(QStringLiteral("ground"));
  m_texture    = m_textureCache.texture(path);
  m_groundRect = m_ground->getRenderedRect();
}

void GroundRenderer::operator()(std::vector<RenderItem>& out, qreal)
{
  RenderItem item;

  item.layer           = RenderLayer::Ground;
  item.texture         = m_texture;
  item.zKey            = 0;
  item.destRect        = QRectF(m_groundRect.topLeft() + m_worldShift, m_groundRect.size());
  item.sourceRectPx    = QRectF(QPointF(0, 0), m_groundRect.size());
  item.maskLocalOrigin = QPointF(0, 0);
  item.usesDaylightMask = true;
  out.push_back(item);
}
