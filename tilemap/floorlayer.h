#ifndef FLOORLAYER_H
#define FLOORLAYER_H

#include "tilelayer.h"

class TileMap;

class FloorLayer : public TileLayer
{
  Q_OBJECT
public:
  explicit FloorLayer(QObject *parent = nullptr);

  void load(const QJsonObject& object, const TileMap* parent);

  TileMap* getTileMap() const { return tilemap; }
  bool isUnderground() const { return underground; }

  void renderToImage(QImage& image, QPoint) override;

private:
  bool          underground;
  unsigned char floor;
  TileMap*      tilemap;
  Tile          dummyTile;
};

#endif // FLOORLAYER_H
