#ifndef  POINT_H
# define POINT_H

# include <QPoint>

struct Point
{
  int x, y;
  unsigned char z;

  operator QPoint() const { return QPoint{x,y}; }
  inline bool operator==(const QPoint& other) const { return x == other.x() && y == other.y(); }
  inline bool operator!=(const QPoint& other) const { return !(*this == other); }
  inline bool operator==(const Point& other) const { return x == other.x && y == other.y && z == other.z; }
  inline bool operator!=(const Point& other) const { return !(*this == other); }
  inline bool operator< (const Point& other) const { return z < other.z || y < other.y || x < other.x; }

  bool isInvalid() const { return x == -1 || y == -1; }
};

#endif
