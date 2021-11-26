#ifndef  LEVELGRID_H
# define LEVELGRID_H

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QMap>
#include <QVector>
#include <QPair>
#include "utils/point.h"

class TileMap;
class TileZone;
class DynamicObject;
class CharacterMovement;
class Doorway;

class LevelGrid : public QObject
{
  Q_OBJECT
public:
  enum CaseFlag
  {
    BlockCase      = 1,
    VerticalWall   = 2,
    HorizontalWall = 4
  };
  Q_ENUM(CaseFlag)

  struct CaseConnection;

  struct CaseContent
  {
    typedef CharacterMovement    Actor;
    char                         hcover = 0, vcover = 0, cover = 0;
    bool                         hwall = false, vwall = false, block = false;
    bool                         occupied = false;
    DynamicObject*               occupant = nullptr;
    Point                        position;
    std::vector<CaseConnection*> connections;
    QVector<TileZone*>           zones;

    bool  isBlocked() const;
    bool  isLinkedTo(QPoint) const;
    bool  operator==(const CaseContent& other) const { return position == other.position; }
    bool  operator==(const CaseContent* other) const { return position == other->position; }
    bool  operator< (const CaseContent& other) const { return position < other.position; }
    float GetCost(const CaseContent&, const Actor*) const { return 1.f; }
    float GoalDistanceEstimate(const CaseContent&) const;
    std::list<CaseContent*> GetSuccessors(const CaseContent* parent, Actor*) const;

    void connectWith(CaseContent*);
    void disconnectFrom(CaseContent*);
    void clearConnections();
    CaseConnection* connectionWith(CaseContent*) const;
  };

  struct CaseConnection
  {
    void connect(CaseContent*, CaseContent*);
    void disconnect();
    bool operator==(const CaseContent* value) const { return pair.first == value || pair.second == value; }
    float getCost() const;
    bool canGoThrough(CharacterMovement* character);
    bool goThrough(CharacterMovement* character);
    CaseContent* getTargetFor(const CaseContent* value) const { return value && pair.first->position == value->position ? pair.second : pair.first; }

    QPair<CaseContent*, CaseContent*> pair;
    Doorway* doorway = nullptr;
  };

  explicit LevelGrid(QObject *parent = nullptr);
  ~LevelGrid();

  void initializeGrid(TileMap*);
  void initializePathfinding();

  Q_INVOKABLE inline QSize   getSize() const { return size; }
  Q_INVOKABLE bool           isOccupied(int x, int y) const;
  Q_INVOKABLE DynamicObject* getOccupant(int x, int y);
  Q_INVOKABLE int            getVisionQuality(int x, int y, int toX, int toY);
  Q_INVOKABLE int            getCaseFlags(int x, int y) const;
  Q_INVOKABLE int            getCoverValue(int x, int y) const;
  Q_INVOKABLE TileMap*       getTilemap() const { return tilemap; }

  bool findPath(Point from, const QVector<Point>& to, QList<Point>& path, CharacterMovement* character);
  bool findPath(Point from, Point to, QList<Point>& path, CharacterMovement* character);
  bool moveObject(DynamicObject*, int x, int y);
  bool insertObject(DynamicObject*, int x, int y);
  void extractObject(DynamicObject*);
  void removeObject(DynamicObject*);
  void triggerZone(CharacterMovement*, int x, int y);
  void triggerZone(CharacterMovement* c, QPoint p) { triggerZone(c, p.x(), p.y()); }
  void registerZone(TileZone*);
  void unregisterZone(TileZone*);
  void registerDoor(Doorway*);
  void unregisterDoor(Doorway*);
  void eachCase(std::function<void (int x, int y, CaseContent&)> callback, QPoint from = QPoint(0,0), QPoint to = QPoint(0,0));
  CaseContent* getGridCase(QPoint value) { return getGridCase(value.x(), value.y()); }
  CaseContent* getGridCase(Point  value) { return getGridCase(value.x, value.y, value.z); }
  CaseContent* getGridCase(int x, int y);
  CaseContent* getGridCase(int x, int y, unsigned char z);
  QVector<TileZone*> getZonesAt(QPoint);

private:
  static void setCaseOccupant(CaseContent&, DynamicObject*);
  void updateObjectVisibility(DynamicObject* object);
  void updateZoneCases(TileZone*);
  void setZoneCases(TileZone*, QVector<QPoint>);

  TileMap*             tilemap = nullptr;
  QSize                size;
  QVector<CaseContent> grid;
  QMap<TileZone*, QVector<CaseContent*>>   zoneCases;
  QMap<TileZone*, QMetaObject::Connection> zoneListener;
};

#endif // LEVELGRID_H
