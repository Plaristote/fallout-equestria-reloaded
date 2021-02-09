#ifndef  LEVELGRID_H
# define LEVELGRID_H

#include <QObject>
#include <QSize>
#include <QPoint>

class TileMap;
class TileZone;
class DynamicObject;

class LevelGrid : public QObject
{
  Q_OBJECT

  struct CaseContent
  {
    bool                      occupied = false;
    DynamicObject*            occupant = nullptr;
    TileZone*                 zone     = nullptr;
    QPoint                    position;
    std::vector<CaseContent*> successors;

    bool  operator==(const CaseContent& other) { return position == other.position; }
    bool  operator==(const CaseContent* other) { return position == other->position; }
    float GetCost(const CaseContent&) const { return 1.f; }
    float GoalDistanceEstimate(const CaseContent&) const;
    std::list<CaseContent*> GetSuccessors(const CaseContent* parent) const;
  };

public:
  explicit LevelGrid(QObject *parent = nullptr);

  void initializeGrid(TileMap*);
  void initializePathfinding();

  Q_INVOKABLE bool           isOccupied(int x, int y) const;
  Q_INVOKABLE DynamicObject* getOccupant(int x, int y);

  bool findPath(QPoint from, QPoint to, QList<QPoint>& path);
  bool moveObject(DynamicObject*, int x, int y);
  void removeObject(DynamicObject*);
  void triggerZone(DynamicObject*, int x, int y);

private:
  CaseContent* getGridCase(int x, int y);
  static void setCaseOccupant(CaseContent&, DynamicObject*);

  QSize                size;
  QVector<CaseContent> grid;
};

#endif // LEVELGRID_H
