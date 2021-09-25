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

  Q_PROPERTY(LevelGrid* grid MEMBER grid CONSTANT)
public:
  explicit GridComponent(QObject *parent = nullptr);

  LevelGrid*                 getGrid() const { return grid; }
  Q_INVOKABLE DynamicObject* getOccupantAt(int x, int y) { return getOccupantAt(QPoint(x, y)); }
  DynamicObject*             getOccupantAt(QPoint);

  void             load(const QJsonObject&);
  virtual void     registerDynamicObject(DynamicObject*);
  virtual void     unregisterDynamicObject(DynamicObject*);

  void setCharacterPosition(Character*, int x, int y);
  Q_INVOKABLE void setObjectPosition(DynamicObject*, int x, int y);
  Q_INVOKABLE QPoint getAdjustedOffsetFor(const DynamicObject*) const;
  Q_INVOKABLE TileLayer* getRoofFor(DynamicObject*) const;
  Q_INVOKABLE QJSValue getDynamicObjectsAt(int x, int y) const;
  Q_INVOKABLE QPoint getRenderPositionForTile(int x, int y);
  Q_INVOKABLE float getDistance(QPoint, QPoint) const;
  Q_INVOKABLE float getDistance(int ax, int ay, int bx, int by) const { return getDistance(QPoint(ax, ay), QPoint(bx, by)); }

  template<typename LIST>
  static void sortByRenderOrder(LIST& list)
  {
    std::sort(list.begin(), list.end(), &GridComponent::isRenderedBefore);
  }

protected slots:
  virtual void onCharacterDied(Character*);
  void         onRoofVisibilityChanged(TileLayer*);

protected:
  void addCharacterObserver(Character*, QMetaObject::Connection);
  static bool isRenderedBefore(const DynamicObject*, const DynamicObject*);

  LevelGrid* grid = nullptr;
private:
  QMap<Character*, QVector<QMetaObject::Connection>> characterObservers;
};

#endif // GRIDCOMPONENT_H
