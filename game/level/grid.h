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

  Q_PROPERTY(LevelGrid* grid MEMBER grid)
public:
  explicit GridComponent(QObject *parent = nullptr);

  LevelGrid*                 getGrid() const { return grid; }
  Q_INVOKABLE DynamicObject* getOccupantAt(int x, int y) { return getOccupantAt(QPoint(x, y)); }
  DynamicObject*             getOccupantAt(QPoint);

  void             load();
  void             registerDynamicObject(DynamicObject*);
  void             unregisterDynamicObject(DynamicObject*);

  void setCharacterPosition(Character*, int x, int y);
  Q_INVOKABLE void setObjectPosition(DynamicObject*, int x, int y);
  Q_INVOKABLE QPoint getAdjustedOffsetFor(DynamicObject*) const;
  Q_INVOKABLE TileLayer* getRoofFor(DynamicObject*) const;
  Q_INVOKABLE QJSValue getDynamicObjectsAt(int x, int y) const;
  Q_INVOKABLE QPoint getRenderPositionForTile(int x, int y);
  Q_INVOKABLE float getDistance(QPoint, QPoint);

protected slots:
  virtual void onCharacterDied(Character*);
  void         onRoofVisibilityChanged(TileLayer*);

protected:
  void addCharacterObserver(Character*, QMetaObject::Connection);

  LevelGrid* grid = nullptr;
private:
  QMap<Character*, QVector<QMetaObject::Connection>> characterObservers;
};

#endif // GRIDCOMPONENT_H
