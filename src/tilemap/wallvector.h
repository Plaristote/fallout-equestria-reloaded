#pragma once
#include <QPoint>
#include <QList>
#include "tile.h"

class TileMap;

struct SortablePoint : public QPoint
{
  SortablePoint() : QPoint() {}
  SortablePoint(const QPoint& cpy) : QPoint(cpy) {}
  bool operator<(const SortablePoint& p) const
  {
    return y() < p.y() || (y() == p.y() && x() < p.x());
  }
};

struct WallGroup
{
  typedef QMap<SortablePoint, const WallGroup*> Map;
  static QList<WallGroup> factory(TileMap&);
  static Map makeWallGroupMap(const QList<WallGroup>&);

  QPoint        startPosition;
  QPoint        endPosition() const { return startPosition + QPoint(length, 0); }
  QList<QPoint> positions() const;
  QPoint        getRenderPosition() const;
  int           length;
  QList<Tile*>  wallTiles;
  QString       prerenderPath;
  QImage        image;
};
