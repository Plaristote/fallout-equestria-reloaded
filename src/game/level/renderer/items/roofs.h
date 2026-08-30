#pragma once
#include "../itemsrenderer.h"
#include <QElapsedTimer>

class RoofsRenderer : public ItemsRendererInterface
{
public:
  RoofsRenderer(const QString& pathPrefix, TextureCache* textureCache)
      : ItemsRendererInterface(textureCache), m_pathPrefix(pathPrefix)
  {
  }

  void initialize();
  void setTilemap(TileMap* tilemap, int floor) override;
  void operator()(std::vector<RenderItem>& out, qreal elapsedMs) override;

private:
  bool updateOpacity(TileLayer* roof, const qreal maxDelta);
  QString prerenderPathFor(TileLayer* roof) const;
  QPointF worldPositionFor(TileLayer* roof) const;
  void render(std::vector<RenderItem>& out, TileLayer* roof);

private:
  QString                  m_pathPrefix;
  qreal                    m_wallHeight;
  QHash<TileLayer*, qreal> m_roofFadeStates;
  QElapsedTimer            m_roofFadeTimer;
};
