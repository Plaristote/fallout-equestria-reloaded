#pragma once
#include "texturecache.h"
#include "../lights.h"
#include <QSGTexture>
#include <QRectF>
#include <QHash>
#include <memory>

class QQuickWindow;

class DaylightMaskRenderer
{
  struct LightLayerMask
  {
    QImage image;
    bool   dirty = true;
  };
public:
  DaylightMaskRenderer(TextureCache*, LightingComponent*);
  ~DaylightMaskRenderer();

  void setTilemap(TileMap *tilemap, int floor);
  void update(QQuickWindow *window);

  QSGTexture* texture() const { return m_maskTexture.get(); }
  QRectF      rect() const { return m_maskRect; }

private:
  LightLayerMask& ensureLightLayerMask(TileLayer *light);
  void             rebuild(QQuickWindow *window);

  TextureCache*      m_textureCache;
  LightingComponent* m_lighting;
  TileMap*           m_tilemap = nullptr;
  int                m_currentFloor = -1;
  bool               m_dirty = true;
  QRect              m_groundRect;
  TileLayer*         m_ground;
  TileLayer*         m_blocks;
  TileLayer*         m_vwalls;
  TileLayer*         m_hwalls;
  QSize              m_mapSize, m_tileSize;
  qreal              m_wallHeight;

  QHash<TileLayer*, LightLayerMask> m_lightMasks;
  std::unique_ptr<QSGTexture>       m_maskTexture;
  QRectF                            m_maskRect;
  QVector<QMetaObject::Connection>  m_lightingConnections;
};
