#ifndef  PREPARECASE_H
# define PREPARECASE_H

# include "levelgrid.h"

class TileMap;
class TileLayer;

struct PrepareCaseFunctor
{
  TileMap*   tilemap;
  TileLayer* ground;
  TileLayer* blocks;
  TileLayer* wallsV;
  TileLayer* wallsH;

  void run(int x, int y, LevelGrid::CaseContent& gridCase);

private:
  void obstacle(int x, int y, LevelGrid::CaseContent& gridCase);
  void cover(LevelGrid::CaseContent& gridCase);
};

#endif // PREPARECASE_H
