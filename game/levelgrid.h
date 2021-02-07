#ifndef  LEVELGRID_H
# define LEVELGRID_H

#include <QObject>
#include <QSize>
#include <QPoint>

class TileMap;
class DynamicObject;

class LevelGrid : public QObject
{
  Q_OBJECT

  struct CaseContent
  {
    bool           occupied = false;
    DynamicObject* occupant = nullptr;
  };
public:

  explicit LevelGrid(QObject *parent = nullptr);

  void initializeGrid(TileMap*);

  Q_INVOKABLE bool           isOccupied(int x, int y) const;
  Q_INVOKABLE DynamicObject* getOccupant(int x, int y);

  bool moveObject(DynamicObject*, int x, int y);

private:
  CaseContent* getGridCase(int x, int y);
  static void setCaseOccupant(CaseContent&, DynamicObject*);

  QSize                size;
  QVector<CaseContent> grid;
};

#endif // LEVELGRID_H
