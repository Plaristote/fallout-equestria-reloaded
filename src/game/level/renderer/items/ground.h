#pragma once
#include "../itemsrenderer.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilemap.h"

class GroundRenderer : public ItemsRendererInterface
{
public:
  GroundRenderer(const QString& pathPrefix, TextureCache* textureCache)
      : ItemsRendererInterface(textureCache), m_pathPrefix(pathPrefix)
  {
  }

  void setTilemap(TileMap* tilemap, int floor) override;
  void operator()(std::vector<RenderItem>& out, qreal) override;

private:
  const QString m_pathPrefix;
  TileLayer*  m_ground;
  QSGTexture* m_texture;
  QRect       m_groundRect;
};
