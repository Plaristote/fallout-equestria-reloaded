#ifndef  GRIDCOMPONENT_H
# define GRIDCOMPONENT_H

# include <QObject>
# include "../levelgrid.h"
# include "../character.h"

class GridComponent : public QObject
{
  Q_OBJECT

  Q_PROPERTY(LevelGrid* grid MEMBER grid)
public:
  explicit GridComponent(QObject *parent = nullptr);

  virtual QPoint getRenderPositionForTile(int x, int y) = 0;

  LevelGrid*                 getGrid() const { return grid; }
  Q_INVOKABLE DynamicObject* getOccupantAt(int x, int y) { return getOccupantAt(QPoint(x, y)); }
  DynamicObject*             getOccupantAt(QPoint);

  void             registerDynamicObject(DynamicObject*);
  void             unregisterDynamicObject(DynamicObject*);
  Q_INVOKABLE bool moveTo(Character* character, int x, int y) { return moveTo(character, QPoint(x, y)); }
  bool             moveTo(Character*, QPoint);

  bool startCharacterMoveToTile(Character*, int x, int y);
  void setCharacterPosition(Character*, int x, int y);
  Q_INVOKABLE void setObjectPosition(DynamicObject*, int x, int y);

protected slots:
  virtual void onMovementFinished(Character*);
  virtual void onCharacterDied(Character*);

protected:
  void addCharacterObserver(Character*, QMetaObject::Connection);

  LevelGrid* grid;
private:
  QMap<Character*, QVector<QMetaObject::Connection>> characterObservers;
};

#endif // GRIDCOMPONENT_H
