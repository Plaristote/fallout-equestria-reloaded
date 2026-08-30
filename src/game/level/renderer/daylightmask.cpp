#include "daylightmask.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilemap.h"
#include <QQuickWindow>
#include <QPainter>

static void stampSilhouette(QImage &mask, const QImage &source, const QRect &sourceClip, const QPointF &destPos)
{
  if (!source.isNull() && !sourceClip.isEmpty())
  {
    QPainter painter(&mask);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(destPos, source.copy(sourceClip));
  }
}

DaylightMaskRenderer::DaylightMaskRenderer(TextureCache* textureCache, LightingComponent* lighting)
    : m_textureCache(textureCache), m_lighting(lighting)
{
  m_lightingConnections << QObject::connect(lighting, &LightingComponent::activeLightsChanged, [this] { m_dirty = true; });
  m_lightingConnections << QObject::connect(lighting, &LightingComponent::lightLayerTilesChanged, [this](TileLayer *layer)
  {
    m_lightMasks.remove(layer);
    m_dirty = true;
  });
}

DaylightMaskRenderer::~DaylightMaskRenderer()
{
  for (const QMetaObject::Connection &c : m_lightingConnections)
    QObject::disconnect(c);
}

void DaylightMaskRenderer::setTilemap(TileMap *tilemap, int floor)
{
  m_tilemap = tilemap;
  m_currentFloor = floor;
  m_lightMasks.clear();
  m_dirty = true;
  m_ground      = m_tilemap->getLayer(QStringLiteral("ground"));
  if (m_ground)
  {
    m_groundRect  = m_ground->getRenderedRect();
    m_blocks      = m_tilemap->getLayer(QStringLiteral("blocks"));
    m_vwalls      = m_tilemap->getLayer(QStringLiteral("walls-v"));
    m_hwalls      = m_tilemap->getLayer(QStringLiteral("walls-h"));
    m_mapSize     = m_tilemap->getSize();
    m_tileSize    = m_tilemap->getTileSize();
    m_wallHeight  = m_tileSize.height() * 2;
  }
  else
  {
    m_groundRect = QRect();
    m_mapSize = m_tileSize = QSize();
    m_blocks = m_vwalls = m_hwalls = nullptr;
    m_wallHeight = 0;
  }
}

void DaylightMaskRenderer::update(QQuickWindow *window)
{
  if (m_dirty) [[unlikely]]
  {
    m_dirty = false;
    rebuild(window);
  }
}

DaylightMaskRenderer::LightLayerMask& DaylightMaskRenderer::ensureLightLayerMask(TileLayer *light)
{
  LightLayerMask& entry = m_lightMasks[light];

  if (entry.dirty && m_tilemap)
  {
    QImage mask(m_groundRect.size(), QImage::Format_ARGB32_Premultiplied);

    mask.fill(Qt::transparent);
    entry.dirty = false;
    for (int y = 0; y < m_mapSize.height(); ++y)
    {
      for (int x = 0; x < m_mapSize.width(); ++x)
      {
        Tile* lightTile = light->getTile(x, y);

        if (lightTile)
        {
          const QPoint renderPosition = m_lighting->getRenderPositionForTile(x, y);
          const QPointF localOrigin = QPointF(renderPosition) - QPointF(m_groundRect.topLeft());

          if (m_ground->getTile(x, y))
            stampSilhouette(mask, m_textureCache->image(lightTile->getImage()), lightTile->getRect(), localOrigin);

          // TODO this might not be how we want walls to be lit
          if (Tile* block = m_blocks ? m_blocks->getTile(x, y) : nullptr)
          {
            const QRect   clip = block->getRect();
            const QSize   size = (m_tileSize - clip.size());
            const QPointF destPosition = localOrigin + QPointF(size.width(), size.height());
            stampSilhouette(mask, m_textureCache->image(block->getImage()), clip, destPosition);
          }
          if (Tile* vwall = m_vwalls ? m_vwalls->getTile(x, y) : nullptr)
          {
            const QPointF destPosition = localOrigin + QPointF(0, -m_wallHeight);
            stampSilhouette(mask, m_textureCache->image(vwall->getImage()), vwall->getRect(), destPosition);
          }
          if (Tile* hwall = m_hwalls ? m_hwalls->getTile(x, y) : nullptr)
          {
            const QRect   clip = hwall->getRect();
            const QSize   size = (m_tileSize - clip.size());
            const QPointF destPosition = localOrigin + QPointF(size.width(), size.height());
            stampSilhouette(mask, m_textureCache->image(hwall->getImage()), clip, destPosition);
          }
        }
      }
    }
    entry.image = std::move(mask);
  }
  return entry;
}

void DaylightMaskRenderer::rebuild(QQuickWindow *window)
{
  m_maskTexture.reset();
  if (m_ground)
  {
    QImage combined(m_groundRect.size(), QImage::Format_ARGB32_Premultiplied);
    combined.fill(Qt::transparent);
    QPainter painter(&combined);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for (TileLayer* light : m_lighting->activeLights())
    {
      const LightLayerMask& entry = ensureLightLayerMask(light);
      if (!entry.image.isNull())
        painter.drawImage(0, 0, entry.image);
    }
    painter.end();
    if (window)
      m_maskTexture.reset(window->createTextureFromImage(combined));
    m_maskRect = QRectF(m_groundRect);
  }
}
