#include "tile.h"
#include "tileset.h"

static QPoint makeRenderPosition(QPoint offset, QPoint currentPosition, QSize tileSize)
{
  return QPoint(
    offset.x() + currentPosition.x() * tileSize.width()  / 2 - currentPosition.y() * tileSize.width()  / 2,
    offset.y() + currentPosition.y() * tileSize.height() / 2 + currentPosition.x() * tileSize.height() / 2
  );
}

Tile::Tile(QObject *parent) : QObject(parent), image(nullptr), tileset(nullptr)
{

}

void Tile::prepare(QPoint offset, const Tileset* tileset, int tid, QPoint position)
{
  this->tileset = tileset;
  this->tid = tid;
  setTexture(&(tileset->getImage()));
  setImage(&tileset->getSource());
  setPosition(position);
  setRect(tileset->getClipRectFor(tid));
  setRenderPosition(makeRenderPosition(offset, position, tileset->getTileSize()));
}

QVariant Tile::getProperty(const QByteArray &name) const
{
  if (tileset)
    return tileset->getProperty(tid, name);
  return QVariant();
}
