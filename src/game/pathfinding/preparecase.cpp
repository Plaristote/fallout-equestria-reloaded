#include "preparecase.h"
#include "tilemap/tilemap.h"

static bool isWall(const TileLayer* layer, int x, int y)
{
  if (layer)
  {
    Tile* tile = layer->getTile(x, y);

    if (tile)
    {
      QVariant doorwayProp = tile->getProperty("doorway");

      return doorwayProp.isNull() || doorwayProp.toBool() == false;
    }
  }
  return false;
}

static char getCoverValue(const TileLayer* layer, const LevelGrid::CaseContent& caseContent, bool hasObstacle)
{
  if (layer)
  {
    Tile* tile = layer->getTile(caseContent.position.x, caseContent.position.y);

    if (tile)
    {
      QVariant coverProp = tile->getProperty("cover");

      if (!coverProp.isNull())
        return static_cast<char>(coverProp.toInt());
    }
  }
  return hasObstacle ? 100 : 0;
}

void PrepareCaseFunctor::run(int x, int y, LevelGrid::CaseContent& gridCase)
{
  obstacle(x, y, gridCase);
  cover(gridCase);
}

void PrepareCaseFunctor::obstacle(int x, int y, LevelGrid::CaseContent& gridCase)
{
  gridCase.occupied = gridCase.block = isWall(blocks, x, y);
  gridCase.hwall    = isWall(wallsH, x, y);
  gridCase.vwall    = isWall(wallsV, x, y);
  gridCase.position = Point{x, y, tilemap->getFloor()};
}

void PrepareCaseFunctor::cover(LevelGrid::CaseContent& gridCase)
{
  if (gridCase.block)
    gridCase.cover  = getCoverValue(blocks, gridCase, gridCase.block);
  else
    gridCase.cover  = getCoverValue(ground, gridCase, gridCase.block);
  if (gridCase.hwall)
    gridCase.hcover = getCoverValue(wallsH, gridCase, gridCase.hwall);
  if (gridCase.vwall)
    gridCase.vcover = getCoverValue(wallsV, gridCase, gridCase.vwall);
}
