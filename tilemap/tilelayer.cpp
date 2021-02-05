#include "tilelayer.h"
#include "tileset.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

TileLayer::TileLayer(QObject *parent) : QObject(parent)
{
}

void TileLayer::load(const QJsonObject& object, const QVector<Tileset*>& tilesets)
{
  name = object["name"].toString();
  size.setWidth(object["width"].toInt());
  size.setHeight(object["height"].toInt());
  offset.setX(object["x"].toInt());
  offset.setY(object["y"].toInt());
  loadTiles(object["data"].toArray(), tilesets);
}

void TileLayer::loadTiles(const QJsonArray& tileArray, const QVector<Tileset*>& tilesets)
{
  QPoint currentPosition(0, 0);

  for (QJsonValue value : tileArray)
  {
    int tid = value.toInt();

    // create the tile
    if (tid > 0)
    {
      for (Tileset* tileset : tilesets)
      {
        if (tileset->isInRange(tid))
        {
          Tile* tile = new Tile(this);

          tile->setImage(&tileset->getSource());
          tile->setPosition(currentPosition);
          tile->setRect(tileset->getClipRectFor(tid));
          tiles.push_back(tile);
          break ;
        }
      }
    }
    else
      tiles.push_back(nullptr);
    if (currentPosition.x() >= size.width())
    {
      currentPosition.setX(0);
      currentPosition.ry()++;
    }
    else
      currentPosition.rx()++;
  }
}

Tile* TileLayer::getTile(int x, int y)
{
  int position = y * size.width() + x;

  if (position >= tiles.count())
    return nullptr;
  return tiles.at(position);
}
