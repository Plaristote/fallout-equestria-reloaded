#include "tilelayer.h"
#include "tileset.h"
#include "tilemap.h"
#include <QImage>
#include <QPainter>
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

          tile->setTexture(&(tileset->getImage()));
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

Tile* TileLayer::getTile(int x, int y) const
{
  int position = y * size.width() + x;

  if (position >= tiles.count() || x < 0 || y < 0)
    return nullptr;
  return tiles.at(position);
}

bool TileLayer::isInside(int x, int y) const
{
  return getTile(x, y) != nullptr;
}

QRect TileLayer::getRenderedRect() const
{
  if (tiles.begin() != tiles.end())
  {
    QPoint min, max;

    for (int x = 0 ; x < size.width() ; ++x)
    {
      for (int y = 0 ; y < size.height() ; ++y)
      {
        Tile* tile = getTile(x, y);

        if (tile)
        {
          QRect renderRect = tile->getRenderRect();

          if (min.x() > renderRect.topLeft().x())
            min.setX(renderRect.topLeft().x());
          if (min.y() > renderRect.topLeft().y())
            min.setY(renderRect.topLeft().y());
          if (max.x() < renderRect.bottomRight().x())
            max.setX(renderRect.bottomRight().x());
          if (max.y() < renderRect.bottomRight().y())
            max.setY(renderRect.bottomRight().y());
        }
      }
    }
    return QRect(QPoint(min), QSize(max.x() - min.x(), max.y() - min.y()));
  }
 return QRect();
}

QSize TileLayer::getRenderedSize() const
{
  if (tiles.begin() != tiles.end())
  {
    QPoint min, max;

    for (int x = 0 ; x < size.width() ; ++x)
    {
      for (int y = 0 ; y < size.height() ; ++y)
      {
        Tile* tile = getTile(x, y);

        if (tile)
        {
          QRect renderRect = tile->getRenderRect();

          if (min.x() > renderRect.topLeft().x())
            min.setX(renderRect.topLeft().x());
          if (min.y() > renderRect.topLeft().y())
            min.setY(renderRect.topLeft().y());
          if (max.x() < renderRect.bottomRight().x())
            max.setX(renderRect.bottomRight().x());
          if (max.y() < renderRect.bottomRight().y())
            max.setY(renderRect.bottomRight().y());
        }
      }
    }
    return QSize(max.x() - min.x(), max.y() - min.y());
  }
  return QSize();
}

void TileLayer::renderToFile(const QString& fileName) const
{
  QRect    renderedRect(getRenderedRect());
  QSize    imageSize = renderedRect.size();
  QPoint   offset(renderedRect.x(), 0);
  QImage   image(imageSize, QImage::Format_ARGB32);
  QPainter painter;

  image.fill(Qt::transparent);
  painter.begin(&image);
  for (int x = 0 ; x < size.width() ; ++x)
  {
    for (int y = 0 ; y < size.height() ; ++y)
    {
      Tile* tile = getTile(x, y);

      if (tile)
      {
        QRect renderRect = tile->getRenderRect();
        QRect relativeRect(renderRect.topLeft() - offset, renderRect.size());

        painter.drawImage(relativeRect, tile->getTexture(), tile->getRect());
      }
    }
  }
  painter.end();
  image.save(fileName);
}
