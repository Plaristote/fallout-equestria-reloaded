#include "floorlayer.h"
#include "tilemap.h"
#include <QJsonArray>

FloorLayer::FloorLayer(QObject *parent) : TileLayer(parent)
{

}

void FloorLayer::load(const QJsonObject& object, const TileMap* parent)
{
  QVector<TileLayer*> allLayers;

  name = "floor_" + object["name"].toString();
  tilemap              = new TileMap(this);
  tilemap->tilesets    = parent->tilesets;
  tilemap->textureList = parent->textureList;
  tilemap->tileSize    = parent->tileSize;
  tilemap->mapSize     = parent->mapSize;
  tilemap->loadLayers(object["layers"].toArray());
  allLayers.append(tilemap->getLayers());
  allLayers.append(tilemap->getRoofs().toVector());
  initialize(tilemap->mapSize);
  dirtyRenderRect = dirtyRenderSize = false;
  renderRect = QRect(0, 0, size.width() * tilemap->getTileSize().width(), size.height() * tilemap->getTileSize().height());
  renderSize = renderRect.size();
  for (int x = 0 ; x < tilemap->getSize().width() ; ++x)
  {
    for (int y = 0 ; y < tilemap->getSize().height() ; ++y)
    {
      for (TileLayer* layer : allLayers)
      {
        if (layer->getTile(x, y) != nullptr)
          tiles[y * size.width() + x] = &dummyTile;
      }
    }
  }
}

void FloorLayer::renderToImage(QImage& image, QPoint)
{
  tilemap->renderToImage(image);
}
