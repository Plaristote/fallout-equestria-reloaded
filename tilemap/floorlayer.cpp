#include "floorlayer.h"
#include "tilemap.h"
#include "properties.h"
#include <QJsonArray>

FloorLayer::FloorLayer(QObject *parent) : TileLayer(parent)
{

}

void FloorLayer::load(const QJsonObject& object, const TileMap* parent)
{
  QVector<TileLayer*> allLayers;

  name = "floor_" + object["name"].toString();
  offset.setX(object["offsetx"].toInt() - parent->tileSize.width() / 2);
  offset.setY(object["offsety"].toInt() - parent->tileSize.height() / 2 + 15);
  tilemap              = new TileMap(this);
  tilemap->tilesets    = parent->tilesets;
  tilemap->textureList = parent->textureList;
  tilemap->tileSize    = parent->tileSize;
  tilemap->mapSize     = parent->mapSize;
  tilemap->floor       = parent->getFloor() + 1;
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
        if (layer->getTile(x, y) != nullptr && !layer->getName().startsWith("wall"))
        {
          tiles[y * size.width() + x] = &dummyTile;
          break ;
        }
      }
    }
  }
}

void FloorLayer::renderToImage(QImage& image, QPoint)
{
  QPoint floorsOffset(getTileMap()->getTileSize().width() / 2 + 2, getTileMap()->getTileSize().height() * 2 + 2);

  tilemap->renderToImage(image,  this->offset + floorsOffset);
}
