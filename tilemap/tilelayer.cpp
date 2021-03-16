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
  offset.setX(object["offsetx"].toInt());
  offset.setY(object["offsety"].toInt());
  loadTiles(object["data"].toArray(), tilesets);
}

static QPoint getRenderPosition(QPoint offset, QPoint currentPosition, QSize tileSize)
{
  return QPoint(
    offset.x() + currentPosition.x() * tileSize.width()  / 2 - currentPosition.y() * tileSize.width()  / 2,
    offset.y() + currentPosition.y() * tileSize.height() / 2 + currentPosition.x() * tileSize.height() / 2
  );
}

void TileLayer::loadTiles(const QJsonArray& tileArray, const QVector<Tileset*>& tilesets)
{
  QPoint currentPosition(0, 0);

  for (const QJsonValue& value : qAsConst(tileArray))
  {
    int tid = value.toInt();

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
          tile->setRenderPosition(getRenderPosition(offset, currentPosition, tileset->getTileSize()));
          tiles.push_back(tile);
          break ;
        }
      }
    }
    else
      tiles.push_back(nullptr);
    if (currentPosition.x() >= size.width() - 1)
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

  if (position >= tiles.count() || x < 0 || y < 0)
    return nullptr;
  return tiles.at(position);
}
