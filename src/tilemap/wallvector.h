#pragma once
#include <QPoint>
#include <QList>
#include "tile.h"

class TileMap;

struct WallGroup
{
  static QList<WallGroup> factory(TileMap&);

  QPoint        startPosition;
  QPoint        endPosition() const { return startPosition + QPoint(length, 0); }
  QList<QPoint> positions() const;
  QPoint        getRenderPosition() const;
  int           length;
  QList<Tile*>  wallTiles;
  QString       prerenderPath;
  QImage        image;
};
