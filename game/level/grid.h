#ifndef  GRIDCOMPONENT_H
# define GRIDCOMPONENT_H

# include <QObject>
# include "debug.h"
# include "../pathfinding/levelgrid.h"
# include "../pathfinding/zonegrid.h"

class TileLayer;

class GridComponent : public DebugComponent
{
  Q_OBJECT
  typedef DebugComponent ParentType;

  Q_PROPERTY(LevelGrid* grid READ getGrid NOTIFY floorChanged)
  Q_PROPERTY(TileMap* tilemap READ getTileMap NOTIFY floorChanged)
  Q_PROPERTY(unsigned int currentFloor READ getCurrentFloor WRITE setCurrentFloor NOTIFY floorChanged)
  Q_PROPERTY(unsigned int floorCount READ getFloorCount NOTIFY floorChanged)
public:
  explicit GridComponent(QObject *parent = nullptr);

  LevelGrid*                 getGrid() const { return floors.at(currentFloor); }
  TileMap*                   getTileMap() const { return grid ? getGrid()->getTilemap() : nullptr; }
  unsigned int               getFloorCount() const { return static_cast<unsigned int>(floors.size()); }
  unsigned int               getCurrentFloor() const { return currentFloor; }
  void                       setCurrentFloor(unsigned int value);
  Q_INVOKABLE DynamicObject* getOccupantAt(int x, int y, unsigned int floor = NULL_FLOOR) { return getOccupantAt(QPoint(x, y), static_cast<unsigned char>(floor)); }
  DynamicObject*             getOccupantAt(QPoint, unsigned char floor);
  const QVector<LevelGrid*>& getFloors() const { return floors; }
  LevelGrid*                 getFloorGrid(unsigned char i) const { return i < floors.size() ? floors.at(i) : nullptr; }
  Q_INVOKABLE LevelGrid*     getFloorGrid(unsigned int i) const { return getFloorGrid(static_cast<unsigned char>(i)); }
  TileZone*                  getDefaultEntryZone() const;

  void             load(const QJsonObject&);
  void             save(QJsonObject&) const;
  virtual void     registerDynamicObject(DynamicObject*);
  virtual void     unregisterDynamicObject(DynamicObject*);

  Q_INVOKABLE void       setCharacterPosition(Character*, int x, int y, unsigned char floor = NULL_FLOOR);
  QPoint                 getTilePosition(QPoint position) const;
  Q_INVOKABLE bool       moveCharacterToZone(Character*, const QString& name, unsigned char floor = NULL_FLOOR);
  Q_INVOKABLE bool       moveCharacterToZone(Character*, TileZone* zone);
  Q_INVOKABLE void       setObjectPosition(DynamicObject*, int x, int y, unsigned char floor = NULL_FLOOR);
  Q_INVOKABLE QPoint     getAdjustedOffsetFor(const DynamicObject*) const;
  Q_INVOKABLE TileLayer* getRoofFor(const DynamicObject*) const;
  Q_INVOKABLE QJSValue   getDynamicObjectsAt(int x, int y, unsigned int floor = NULL_FLOOR) const;
  Q_INVOKABLE QPoint     getRenderPositionForTile(int x, int y, unsigned char z = NULL_FLOOR);
  Q_INVOKABLE float      getDistance(QPoint, QPoint) const;
  Q_INVOKABLE float      getDistance(int ax, int ay, int bx, int by) const { return getDistance(QPoint(ax, ay), QPoint(bx, by)); }
  Q_INVOKABLE int        getVisionQuality(int ax, int ay, int bx, int by) const;

  template<typename LIST>
  static void sortByRenderOrder(LIST& list)
  {
    std::sort(list.begin(), list.end(), &GridComponent::isRenderedBefore);
  }

  ZoneGrid& getPathfinder() { return pathfinding; }

signals:
  void floorChanged();

protected slots:
  virtual void onCharacterDied(Character*);
  void         onRoofVisibilityChanged(TileLayer*);
  void         onPathBlockedChanged(DynamicObject*);

protected:
  void addCharacterObserver(Character*, QMetaObject::Connection);
  void setGridObjectPosition(DynamicObject*, int x, int y, unsigned char floor);
  void setRenderObjectPosition(DynamicObject*, int x, int y);
  void setBlockPathBeahviour(DynamicObject*, bool blockPath);
  static bool isRenderedBefore(const DynamicObject*, const DynamicObject*);

  LevelGrid* grid = nullptr;
private:
  QMap<Character*, QVector<QMetaObject::Connection>> characterObservers;
  QMap<DynamicObject*, QVector<QMetaObject::Connection>> objectObservers;
  unsigned char currentFloor = 0;
  QVector<LevelGrid*> floors;
  ZoneGrid pathfinding;
};

#endif // GRIDCOMPONENT_H
