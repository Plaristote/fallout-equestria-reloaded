#ifndef  POINT_H
# define POINT_H

# include <QPoint>

struct Point
{
  int x, y;
  unsigned char z;

  operator QPoint() const { return QPoint{x,y}; }
  bool operator==(const QPoint& other) const { return x == other.x() && y == other.y(); }
  bool operator==(const Point& other) const { return x == other.x && y == other.y && z == other.z; }
};

#endif
