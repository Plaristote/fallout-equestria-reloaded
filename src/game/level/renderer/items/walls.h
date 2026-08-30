#pragma once
#include "../itemsrenderer.h"
#include "game/level/camera.h"

class WallsRenderer : public ItemsRendererInterface
{
public:
  WallsRenderer(TextureCache* textureCache, CameraComponent* camera)
      : ItemsRendererInterface(textureCache), m_camera(camera)
  {
  }

  void setTilemap(TileMap* tilemap, int floor) override;
  void operator()(std::vector<RenderItem>& out, qreal) override;

private:
  void renderWallsAt(std::vector<RenderItem>& out, QPoint position);
  void renderWallsAt(std::vector<RenderItem>& out, QPoint position, Tile* block, Tile* vwall, Tile* hwall);
  void pushWallFace(std::vector<RenderItem>& out, Tile* wall, QPointF facePos, int64_t zKey, bool cropEligible);

  QPointF blockRenderPosition(Tile* block) const;
  QPointF vwallRenderPosition(Tile*) const;
  QPointF hwallRenderPosition(Tile* hwall) const;

  TileLayer*       m_ground;
  TileLayer*       m_blocks;
  TileLayer*       m_vwalls;
  TileLayer*       m_hwalls;
  QPointF          m_groundTopLeft;
  QSize            m_tileSize;
  qreal            m_wallHeight;
  CameraComponent* m_camera;
  Character*       m_player = nullptr;
  QPoint           m_playerPos;
};
