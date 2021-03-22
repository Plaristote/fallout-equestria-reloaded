#include "tilemapimageprovider.h"
#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include <QPainter>
#include <QPaintEngine>
#include <QImage>

TileMapImageProvider::TileMapImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

QPixmap TileMapImageProvider::requestPixmap(const QString& id, QSize* size, const QSize&)
{
  if (tilemap)
  {
    auto       nameParts = id.split("/");
    TileLayer* layer = nullptr;

    if (nameParts.front() == "roofs")
      layer = tilemap->getRoofLayer(nameParts[1]);
    else if (nameParts.front() == "layers")
      layer = tilemap->getLayer(nameParts[1]);
    if (layer)
    {
      QRect   zone;
      QImage  image;

      image.fill(Qt::transparent);
      zone.setTop(nameParts[2].toInt());
      zone.setLeft(nameParts[3].toInt());
      zone.setWidth(nameParts[4].toInt());
      zone.setHeight(nameParts[5].toInt());
      if (size)
        *size = QSize(zone.width(), zone.height());
      renderTileLayer(layer, image.paintEngine(), zone);
      return QPixmap::fromImage(image);
    }
  }
  return QPixmap();
}

void TileMapImageProvider::renderTileLayer(TileLayer* layer, QPaintEngine* painter, QRect zone)
{
  QSize mapSize(tilemap->getSize());

  for (int x = 0 ; x < mapSize.width() ; ++x)
  {
    for (int y = 0 ; y < mapSize.height() ; ++y)
    {
      Tile* tile = layer->getTile(x, y);
      QRect rect = tile->getRenderRect();

      if (tile && zone.intersects(rect))
      {
        QRect relativeRect(
          QPoint(rect.x() - zone.x(), rect.y() - zone.y()),
          rect.size()
        );

        painter->drawImage(relativeRect, tile->getTexture(), tile->getRect());
      }
    }
  }
}
