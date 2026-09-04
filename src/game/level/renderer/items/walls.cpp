#include "walls.h"
#include "../zindex.h"

void WallsRenderer::setTilemap(TileMap* tilemap, int floor)
{
  ItemsRendererInterface::setTilemap(tilemap, floor);
  m_player = m_camera->getPlayer();
  Q_ASSERT(tilemap != nullptr);
  m_ground        = tilemap->getLayer(QStringLiteral("ground"));
  m_blocks        = tilemap->getLayer(QStringLiteral("blocks"));
  m_vwalls        = tilemap->getLayer(QStringLiteral("walls-v"));
  m_hwalls        = tilemap->getLayer(QStringLiteral("walls-h"));
  m_mapSize       = tilemap->getSize();
  m_tileSize      = tilemap->getTileSize();
  m_wallHeight    = m_tileSize.height() * 2;
  m_groundTopLeft = QPointF(m_ground->getRenderedRect().topLeft());
  m_currentFloor  = floor;
}

void WallsRenderer::operator()(std::vector<RenderItem>& out, qreal)
{
  if (m_ground)
  {
    const int mapWidth = m_mapSize.width();

    m_playerPos = m_player ? m_player->getPosition() : QPoint();
    for (int ty = 0; ty < m_mapSize.height(); ++ty)
    {
      for (int tx = 0; tx < m_mapSize.width(); ++tx)
      {
        QPoint position(tx, ty);

        if (m_camera->isWallRendered(position))
          renderWallsAt(out, position);
      }
    }
  }
}

void WallsRenderer::renderWallsAt(std::vector<RenderItem>& out, QPoint position)
{
  Tile* block = m_blocks ? m_blocks->getTile(position.x(), position.y()) : nullptr;
  Tile* vwall = m_vwalls ? m_vwalls->getTile(position.x(), position.y()) : nullptr;
  Tile* hwall = m_hwalls ? m_hwalls->getTile(position.x(), position.y()) : nullptr;

  if (block || vwall || hwall)
    renderWallsAt(out, position, block, vwall, hwall);
}

void WallsRenderer::renderWallsAt(std::vector<RenderItem>& out, QPoint position, Tile* block, Tile* vwall, Tile* hwall)
{
  const QPoint  renderPosition    = m_camera->getRenderPositionForTile(position.x(), position.y(), m_currentFloor);
  const bool    hasVOrH           = (vwall || hwall);
  const int64_t zKey              = ZOrder::forWall(position.x(), position.y(), m_mapSize.width(), hasVOrH);
  const bool    vwallCropEligible = position.x() >= m_playerPos.x();
  const bool    hwallCropEligible = position.y() >= m_playerPos.y();
  const bool    blockCropEligible = vwallCropEligible || hwallCropEligible;

  if (block)
    pushWallFace(out, block, renderPosition + blockRenderPosition(block), zKey, blockCropEligible);
  if (vwall)
    pushWallFace(out, vwall, renderPosition + vwallRenderPosition(vwall), zKey, vwallCropEligible);
  if (hwall)
    pushWallFace(out, hwall, renderPosition + hwallRenderPosition(hwall), zKey, hwallCropEligible);
}

void WallsRenderer::pushWallFace(std::vector<RenderItem>& out, Tile* wall, QPointF facePos, int64_t zKey, bool cropEligible)
{
  const QRect clip = wall->getRect();
  RenderItem item;

  item.layer                 = RenderLayer::Wall;
  item.texture               = m_textureCache.texture(wall->getImage());
  item.zKey                  = zKey;
  item.destRect              = QRectF(facePos + m_worldShift, clip.size());
  item.maskLocalOrigin       = facePos - m_groundTopLeft;
  item.sourceRectPx          = clip;
  item.eligibleForCropCircle = cropEligible;
  item.usesDaylightMask      = true;
  if (item.texture)
    out.push_back(item);
}

QPointF WallsRenderer::blockRenderPosition(Tile* block) const
{
  return QPointF(m_tileSize.width() - block->getRect().width(),
                  m_tileSize.height() - block->getRect().height());
}

QPointF WallsRenderer::vwallRenderPosition(Tile*) const
{
  return QPointF(0, -m_wallHeight);
}

QPointF WallsRenderer::hwallRenderPosition(Tile* hwall) const
{
  return QPointF(m_tileSize.width() - hwall->getRect().width(),
                  m_tileSize.height() - hwall->getRect().height());
}
