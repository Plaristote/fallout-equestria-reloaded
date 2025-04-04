#ifndef  ZONEGRID_H
# define ZONEGRID_H

# include "pathzone.h"
# include "levelgrid.h"

class LevelGrid;

class ZoneGrid
{
public:
  ZoneGrid();

  void prepareZoneGrid(QVector<LevelGrid*> levels);
  bool findPath(Point from, Point to, QList<Point>& path, CharacterMovement* character);
  bool findPath(Point from, const QVector<Point> &to, QList<Point> &path, CharacterMovement *character, bool quickMode = false);
  void connectCases(Point, Point);
  void disconnectCases(Point, Point);
  int actionPointCost(Point, Point);

  LevelGrid::CaseContent* getGridCase(Point);
  PathZone*               getPathZone(Point);
  PathZone*               getPathZoneById(unsigned int);

  QVector<LevelGrid*> levels;
  QVector<PathZone>   zones;
};

#endif // ZONEGRID_H
