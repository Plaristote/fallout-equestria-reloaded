#include "tilemap.h"
#include <QPainter>

struct TileRenderFunctor
{
  QPainter& painter;
  Tile*     tile;
  QPoint    position, renderOffset;
  QSize     tileSize;

  void operator()()
  {
    QRect  renderRect = tile->getRenderRect();
    QPoint relativePos(position - QPoint(0, tile->getRect().height() - tileSize.height()));
    QRect  relativeRect(relativePos - renderOffset, renderRect.size());

    painter.drawImage(relativeRect, tile->getTexture(), tile->getRect());
  }
};

static void renderTilemapWalls(TileMap& tilemap, QImage& image, QPoint renderOffset)
{
  QPainter painter;
  QSize mapSize = tilemap.getSize();
  TileLayer* vwalls = tilemap.getLayer("walls-v");
  TileLayer* hwalls = tilemap.getLayer("walls-h");
  TileLayer* blocks = tilemap.getLayer("blocks");

  painter.begin(&image);
  for (int x = 0 ; x < mapSize.width() ; ++x)
  {
    for (int y = 0 ; y < mapSize.height() ; ++y)
    {
      Tile* block = blocks ? blocks->getTile(x, y) : nullptr;
      Tile* vwall = vwalls ? vwalls->getTile(x, y) : nullptr;
      Tile* hwall = hwalls ? hwalls->getTile(x, y) : nullptr;
      QPoint position(tilemap.getPointFor(x, y));

      for (Tile* tile : QVector<Tile*>{block, vwall, hwall})
      {
        if (tile)
          TileRenderFunctor{painter, tile, position, renderOffset, tilemap.getTileSize()}();
      }
    }
  }
  painter.end();
}

void TileMap::renderToImage(QImage& image, QPoint offset)
{
  QPoint renderOffset = QPoint(-image.width() / 2, 0) - offset;
  TileLayer* ground = getLayer("ground");

  if (ground)
    ground->renderToImage(image, renderOffset);
  renderTilemapWalls(*this, image, renderOffset);
  for (TileLayer* roof : qAsConst(roofs))
    roof->renderToImage(image, renderOffset);
}
