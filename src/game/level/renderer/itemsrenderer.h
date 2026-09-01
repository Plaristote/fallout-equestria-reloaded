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

  inline void run(std::vector<RenderItem>& out, qreal elapsedMs)
  {
    if (m_enabled) [[likely]]
      operator()(out, elapsedMs);
  }

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

  inline bool isEnabled() const { return m_enabled; }
  inline void setEnabled(bool value) { m_enabled = value; }

protected:
  TextureCache& m_textureCache;
  QPointF       m_worldShift;
  TileMap*      m_tilemap;
  QSizeF        m_mapSize;
  int           m_currentFloor;
  bool          m_enabled = true;
};
