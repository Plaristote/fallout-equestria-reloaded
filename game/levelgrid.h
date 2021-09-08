#ifndef  LEVELGRID_H
# define LEVELGRID_H

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QMap>
#include <QVector>

class TileMap;
class TileZone;
class DynamicObject;
class CharacterMovement;

class LevelGrid : public QObject
{
  Q_OBJECT

public:
  struct CaseContent
  {
    char                      hcover = 0, vcover = 0, cover = 0;
    bool                      hwall = false, vwall = false, block = false;
    bool                      occupied = false;
    DynamicObject*            occupant = nullptr;
    QPoint                    position;
    std::vector<CaseContent*> successors;
    QVector<TileZone*>        zones;

    bool  isBlocked() const;
    bool  isLinkedTo(QPoint) const;
    bool  operator==(const CaseContent& other) { return position == other.position; }
    bool  operator==(const CaseContent* other) { return position == other->position; }
    float GetCost(const CaseContent&) const { return 1.f; }
    float GoalDistanceEstimate(const CaseContent&) const;
    std::list<CaseContent*> GetSuccessors(const CaseContent* parent) const;
  };

  explicit LevelGrid(QObject *parent = nullptr);

  void initializeGrid(TileMap*);
  void initializePathfinding();

  Q_INVOKABLE inline QSize   getSize() const { return size; }
  Q_INVOKABLE bool           isOccupied(int x, int y) const;
  Q_INVOKABLE DynamicObject* getOccupant(int x, int y);
  Q_INVOKABLE int            getVisionQuality(int x, int y, int toX, int toY);
  Q_INVOKABLE int            getCaseFlags(int x, int y) const;

  bool findPath(QPoint from, QPoint to, QList<QPoint>& path);
  bool moveObject(DynamicObject*, int x, int y);
  void removeObject(DynamicObject*);
  void triggerZone(CharacterMovement*, int x, int y);
  void triggerZone(CharacterMovement* c, QPoint p) { triggerZone(c, p.x(), p.y()); }
  void registerZone(TileZone*);
  void unregisterZone(TileZone*);
  void eachCase(std::function<void (int x, int y, CaseContent&)> callback, QPoint from = QPoint(0,0), QPoint to = QPoint(0,0));
  CaseContent* getGridCase(int x, int y);
  QVector<TileZone*> getZonesAt(QPoint);

private:
  static void setCaseOccupant(CaseContent&, DynamicObject*);
  void updateObjectVisibility(DynamicObject* object);
  void updateZoneCases(TileZone*);
  void setZoneCases(TileZone*, QVector<QPoint>);

  QSize                size;
  QVector<CaseContent> grid;
  QMap<TileZone*, QVector<CaseContent*>>   zoneCases;
  QMap<TileZone*, QMetaObject::Connection> zoneListener;
};

#endif // LEVELGRID_H
