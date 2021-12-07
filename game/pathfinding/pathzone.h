#ifndef  PATHZONE_H
# define PATHZONE_H

# include <QRect>
# include <QVector>
# include <QMap>
# include <list>
# include "utils/point.h"
# include "levelgrid.h"

class CharacterMovement;

struct PathZone
{
  typedef CharacterMovement Actor;
  typedef QPair<LevelGrid::CaseContent*, LevelGrid::CaseContent*> Connection;
  typedef QVector<Connection> Connections;

  unsigned int       id;
  QRect              rect;
  QVector<QPoint>    positions;
  unsigned char      floor;
  Connections                  exits;
  QMap<PathZone*, Connections> connections;

  bool contains(Point point) const
  {
    return floor == point.z && rect.contains(point) && positions.contains(point);
  }

  bool neighbors(Point point) const
  {
    for (auto it = connections.begin() ; it != connections.end() ; ++it)
    {
      if (it.key()->contains(point))
        return true;
    }
    return false;
  }

  void               addExitTowards(PathZone* targetZone, Connection);
  void               removeExitTowards(PathZone* targetZone, Connection);
  bool               isConnected(const PathZone* target, Actor*) const;
  const Connections* getExitsTowards(const PathZone* target) const;

  operator Point() const { return {rect.topLeft().x(), rect.topLeft().y(), floor}; }
  bool  operator==(unsigned int otherId)  const { return otherId == id; }
  bool  operator==(const PathZone& other) const { return other.id == id; }
  bool  operator==(const PathZone* other) const { return other->id == id; }
  bool  operator< (const PathZone& other) const { return Point(*this) < Point(other); }
  float GetCost(const PathZone&, const Actor*) { return 1; }
  float GoalDistanceEstimate(const PathZone&);
  std::list<PathZone*> GetSuccessors(const PathZone* parent, Actor*) const;
};

#endif // PATHZONE_H
