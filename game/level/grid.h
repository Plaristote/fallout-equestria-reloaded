#ifndef  GRIDCOMPONENT_H
# define GRIDCOMPONENT_H

# include <QObject>
# include "levelbase.h"
# include "../levelgrid.h"

class TileLayer;

class GridComponent : public LevelBase
{
  Q_OBJECT

  Q_PROPERTY(LevelGrid* grid MEMBER grid)
public:
  explicit GridComponent(QObject *parent = nullptr);

  virtual Character* getPlayer() = 0;
  virtual QPoint getRenderPositionForTile(int x, int y) = 0;

  LevelGrid*                 getGrid() const { return grid; }
  Q_INVOKABLE DynamicObject* getOccupantAt(int x, int y) { return getOccupantAt(QPoint(x, y)); }
  DynamicObject*             getOccupantAt(QPoint);

  void             load();
  void             registerDynamicObject(DynamicObject*);
  void             unregisterDynamicObject(DynamicObject*);
  void             registerZone(TileZone*);
  void             unregisterZone(TileZone*);

  void setCharacterPosition(Character*, int x, int y);
  Q_INVOKABLE void setObjectPosition(DynamicObject*, int x, int y);
  Q_INVOKABLE QPoint getAdjustedOffsetFor(DynamicObject*) const;
  Q_INVOKABLE TileLayer* getRoofFor(DynamicObject*) const;

signals:
  void exitZoneEntered(TileZone*);

protected slots:
  virtual void onCharacterDied(Character*);
  void         onZoneEntered(DynamicObject*, TileZone*);
  void         onZoneExited(DynamicObject*, TileZone*);
  void         onRoofVisibilityChanged(TileLayer*);

protected:
  void addCharacterObserver(Character*, QMetaObject::Connection);

  LevelGrid* grid = nullptr;
private:
  QMap<Character*, QVector<QMetaObject::Connection>> characterObservers;
};

#endif // GRIDCOMPONENT_H
