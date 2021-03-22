#ifndef TILEMAPIMAGEPROVIDER_H
#define TILEMAPIMAGEPROVIDER_H

#include <QQuickImageProvider>

class TileMap;
class TileLayer;
class QPaintEngine;

class TileMapImageProvider : public QQuickImageProvider
{
public:
  TileMapImageProvider();

  void setTilemap(TileMap* tilemap)
  {
    this->tilemap = tilemap;
  }

  QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
  void renderTileLayer(TileLayer* layer, QPaintEngine* painter, QRect zone);

  TileMap* tilemap = nullptr;
};

#endif // TILEMAPIMAGEPROVIDER_H
