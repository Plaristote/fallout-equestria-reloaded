#ifndef  GRIDCOMPONENT_H
# define GRIDCOMPONENT_H

# include <QObject>
# include "levelbase.h"
# include "../levelgrid.h"

class TileLayer;

class GridComponent : public LevelBase
{
  Q_OBJECT
  typedef LevelBase ParentType;

  Q_PROPERTY(LevelGrid* grid READ getGrid NOTIFY floorChanged)
  Q_PROPERTY(TileMap* tilemap READ getTileMap NOTIFY floorChanged)
  Q_PROPERTY(unsigned int currentFloor READ getCurrentFloor WRITE setCurrentFloor NOTIFY floorChanged)
public:
  explicit GridComponent(QObject *parent = nullptr);

  LevelGrid*                 getGrid() const { return floors.at(currentFloor); }
  TileMap*                   getTileMap() const { return grid ? getGrid()->getTilemap() : nullptr; }
  unsigned int               getCurrentFloor() const { return currentFloor; }
  void                       setCurrentFloor(unsigned short value) { currentFloor = static_cast<unsigned char>(value); emit floorChanged(); }
  Q_INVOKABLE DynamicObject* getOccupantAt(int x, int y) { return getOccupantAt(QPoint(x, y)); }
  DynamicObject*             getOccupantAt(QPoint);
  const QVector<LevelGrid*>& getFloors() const { return floors; }
  LevelGrid*                 getFloorGrid(unsigned char i) const { return i < floors.size() ? floors.at(i) : nullptr; }
  LevelGrid*                 getFloorGrid(unsigned int i) const { return getFloorGrid(static_cast<unsigned char>(i)); }


  void             load(const QJsonObject&);
  virtual void     registerDynamicObject(DynamicObject*);
  virtual void     unregisterDynamicObject(DynamicObject*);

  void                   setCharacterPosition(Character*, int x, int y, unsigned char floor = NULL_FLOOR);
  QPoint                 getTilePosition(QPoint position) const;
  Q_INVOKABLE bool       moveCharacterToZone(Character*, const QString& name, unsigned char floor = NULL_FLOOR);
  Q_INVOKABLE bool       moveCharacterToZone(Character*, TileZone* zone);
  Q_INVOKABLE void       setObjectPosition(DynamicObject*, int x, int y, unsigned char floor = NULL_FLOOR);
  Q_INVOKABLE QPoint     getAdjustedOffsetFor(const DynamicObject*) const;
  Q_INVOKABLE TileLayer* getRoofFor(DynamicObject*) const;
  Q_INVOKABLE QJSValue   getDynamicObjectsAt(int x, int y) const;
  Q_INVOKABLE QPoint     getRenderPositionForTile(int x, int y);
  Q_INVOKABLE float      getDistance(QPoint, QPoint) const;
  Q_INVOKABLE float      getDistance(int ax, int ay, int bx, int by) const { return getDistance(QPoint(ax, ay), QPoint(bx, by)); }
  Q_INVOKABLE int        getVisionQuality(int ax, int ay, int bx, int by) const;

  template<typename LIST>
  static void sortByRenderOrder(LIST& list)
  {
    std::sort(list.begin(), list.end(), &GridComponent::isRenderedBefore);
  }

signals:
  void floorChanged();

protected slots:
  virtual void onCharacterDied(Character*);
  void         onRoofVisibilityChanged(TileLayer*);

protected:
  void addCharacterObserver(Character*, QMetaObject::Connection);
  void setGridObjectPosition(DynamicObject*, int x, int y, unsigned char floor);
  void setRenderObjectPosition(DynamicObject*, int x, int y);
  static bool isRenderedBefore(const DynamicObject*, const DynamicObject*);

  LevelGrid* grid = nullptr;
private:
  QMap<Character*, QVector<QMetaObject::Connection>> characterObservers;
  unsigned char currentFloor = 0;
  QVector<LevelGrid*> floors;
};

#endif // GRIDCOMPONENT_H
