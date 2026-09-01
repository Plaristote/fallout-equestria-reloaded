#pragma once
#include "texturecache.h"
#include "types.h"
#include "tilemap/tilemap.h"
#include "../fadeduration.h"

class ItemsRendererInterface
{
protected:
  static constexpr qreal g_fadeDurationMs = FADE_DURATION_MS;
public:
  ItemsRendererInterface(TextureCache* textureCache)
    : m_textureCache(*textureCache)
  {
  }

  virtual ~ItemsRendererInterface() = default;

  virtual void operator()(std::vector<RenderItem>& out, qreal elapsedMs) = 0;

  virtual void setTilemap(TileMap* tilemap, int floor)
  {
    Q_ASSERT(tilemap != nullptr);
    m_tilemap      = tilemap;
    m_mapSize      = tilemap->getSize();
    m_currentFloor = floor;
  }

  void setWorldShiftX(qreal value)
  {
    m_worldShift = QPointF(value, 0);
  }

protected:
  TextureCache& m_textureCache;
  QPointF       m_worldShift;
  TileMap*      m_tilemap;
  QSizeF        m_mapSize;
  int           m_currentFloor;
};
