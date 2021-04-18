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
  for (QJsonValue property : object["properties"].toArray())
  {
    QString propertyName = property["name"].toString();
    QJsonValue value = property["value"];

    if (propertyName == "color")
      color = QColor(value.toString());
  }
}

static QPoint getRenderPosition(QPoint offset, QPoint currentPosition, QSize tileSize)
{
  return QPoint(
    offset.x() + currentPosition.x() * tileSize.width()  / 2 - currentPosition.y() * tileSize.width()  / 2,
    offset.y() + currentPosition.y() * tileSize.height() / 2 + currentPosition.x() * tileSize.height() / 2
  );
}

void TileLayer::initialize(QSize size)
{
  this->size = size;
  tiles.fill(nullptr, size.width() * size.height());
  dirtyRenderRect = dirtyRenderSize = true;
}

void TileLayer::clear()
{
  for (auto it = tiles.begin() ; it != tiles.end() ; ++it)
  {
    Tile* tile = *it;

    if (tile != nullptr)
    {
      tile->deleteLater();
      *it = nullptr;
    }
  }
  dirtyRenderRect = dirtyRenderSize = true;
}

void TileLayer::fill(Tileset* tileset, int tileId)
{
  if (tileset && tileId > 0)
  {
    for (auto it = tiles.begin() ; it != tiles.end() ; ++it)
    {
      Tile* tile = *it;

      if (!tile)
      {
        tile = new Tile(this);
        *it = tile;
      }
      prepareTile(tile, tileset, tileId, tile->getPosition());
    }
    dirtyRenderRect = dirtyRenderSize = true;
  }
  else
    clear();
}

void TileLayer::fill(QRect rect, Tileset *tileset, int tileId)
{
  for (int x = rect.x() ; x <= rect.right() ; ++x)
  {
    for (int y = rect.y() ; y <= rect.bottom() ; ++y)
      setTileIdAt(x, y, tileset, tileId);
  }
}

void TileLayer::setTileIdAt(int x, int y, Tileset *tileset, int tileId)
{
  int position = y * size.width() + x;

  if (position < tiles.count() && x >= 0 && y >= 0)
  {
    Tile* tile = tiles.at(position);

    if (tile && (tileset == nullptr || tileId <= 0))
    {
      tile->deleteLater();
      tiles[position] = nullptr;
    }
    else if (tileId > 0 && tileset)
    {
      QPoint coordinates(x, y);

      if (!tile)
      {
        tile = new Tile(this);
        tile->setPosition(coordinates);
        tiles[position] = tile;
      }
      prepareTile(tile, tileset, tileId, coordinates);
    }
  }
  dirtyRenderRect = dirtyRenderSize = true;
}

void TileLayer::prepareTile(Tile* tile, const Tileset* tileset, int tid, QPoint position)
{
  tile->setTexture(&(tileset->getImage()));
  tile->setImage(&tileset->getSource());
  tile->setPosition(position);
  tile->setRect(tileset->getClipRectFor(tid));
  tile->setRenderPosition(getRenderPosition(offset, position, tileset->getTileSize()));
}

void TileLayer::loadTiles(const QJsonArray& tileArray, const QVector<Tileset*>& tilesets)
{
  QPoint currentPosition(0, 0);

  tiles.reserve(tileArray.size());
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

          prepareTile(tile, tileset, tid, currentPosition);
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

QRect TileLayer::getRenderedRect()
{
  if (dirtyRenderRect)
    prepareRenderRect();
  return renderRect;
}

QSize TileLayer::getRenderedSize()
{
  if (dirtyRenderSize)
    prepareRenderSize();
  return renderSize;
}

void TileLayer::prepareRenderRect()
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
    renderRect = QRect(QPoint(min), QSize(max.x() - min.x(), max.y() - min.y()));
  }
  else
    renderRect = QRect();
  dirtyRenderRect = false;
}

void TileLayer::prepareRenderSize()
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
    renderSize = QSize(max.x() - min.x(), max.y() - min.y());
  }
  else
    renderSize = QSize();
  dirtyRenderSize = false;
}

void TileLayer::renderToFile(const QString& fileName)
{
  QRect    renderedRect(getRenderedRect());
  QSize    imageSize = renderedRect.size();
  QImage   image(imageSize, QImage::Format_ARGB32);

  image.fill(Qt::transparent);
  renderToImage(image, QPoint(renderedRect.x(), 0));
  image.save(fileName);
  prerendered = true;
}

void TileLayer::renderToImage(QImage &image, QPoint offset)
{
  QPainter painter;

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
  if (color != Qt::transparent)
  {
    QImage colorLayer(image.size(), QImage::Format_ARGB32);

    for (int x = 0 ; x < image.size().width() ; ++x)
    {
      for (int y = 0 ; y < image.size().height() ; ++y)
      {
        if (image.pixelColor(x, y) != Qt::transparent)
          colorLayer.setPixelColor(x, y, color);
      }
    }
    painter.drawImage(0, 0, colorLayer);
  }
  painter.end();
}
