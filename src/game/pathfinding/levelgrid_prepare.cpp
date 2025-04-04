#include "levelgrid.h"
#include "preparecase.h"
#include "tilemap/tilemap.h"

static bool hasTile(const TileLayer& ground, const LevelGrid::CaseContent& caseContent)
{
  return ground.getTile(caseContent.position.x, caseContent.position.y);
}

static bool isCaseAvailable(const TileLayer* ground, const LevelGrid::CaseContent* caseContent)
{
  return caseContent && !caseContent->occupied && hasTile(*ground, *caseContent);
}

void LevelGrid::initializeGrid(TileMap* tilemap_)
{
  PrepareCaseFunctor functor;

  tilemap = tilemap_;
  functor.tilemap = tilemap_;
  functor.ground  = tilemap->getLayer("ground");
  functor.blocks  = tilemap->getLayer("blocks");
  functor.wallsV  = tilemap->getLayer("walls-v");
  functor.wallsH  = tilemap->getLayer("walls-h");
  size = tilemap->getSize();
  grid.resize(size.width() * size.height());
  eachCase(std::bind(&PrepareCaseFunctor::run, &functor, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  initializePathfinding();
}

void LevelGrid::initializePathfinding()
{
  auto* ground     = tilemap->getLayer("ground");
  auto isAvailable = std::bind(isCaseAvailable, ground, std::placeholders::_1);

  if (!ground)
    return ;
  for (auto& gridCase : grid)
  {
    gridCase.position.z = tilemap->getFloor();
    gridCase.clearConnections();
    if (!isAvailable(&gridCase))
      continue ;

    bool hasLeft  = gridCase.position.x - 1 >= 0;
    bool hasRight = gridCase.position.x + 1 < size.width();
    bool hasUp    = gridCase.position.y - 1 >= 0;
    bool hasDown  = gridCase.position.y + 1 < size.height();

    auto* upLeft    = hasLeft && hasUp    ? getGridCase(gridCase.position.x - 1, gridCase.position.y - 1) : nullptr;
    auto* up        = hasUp               ? getGridCase(gridCase.position.x,     gridCase.position.y - 1) : nullptr;
    auto* upRight   = hasRight && hasUp   ? getGridCase(gridCase.position.x + 1, gridCase.position.y - 1) : nullptr;
    auto* left      = hasLeft             ? getGridCase(gridCase.position.x - 1, gridCase.position.y)     : nullptr;
    auto* right     = hasRight            ? getGridCase(gridCase.position.x + 1, gridCase.position.y)     : nullptr;
    auto* downLeft  = hasDown && hasLeft  ? getGridCase(gridCase.position.x - 1, gridCase.position.y + 1) : nullptr;
    auto* down      = hasDown             ? getGridCase(gridCase.position.x,     gridCase.position.y + 1) : nullptr;
    auto* downRight = hasDown && hasRight ? getGridCase(gridCase.position.x + 1, gridCase.position.y + 1) : nullptr;

    bool upWalled        = up && up->hwall;
    bool upLeftWalled    = (upLeft  && (upLeft->hwall || upLeft->vwall)) || (left && left->vwall) || upWalled;
    bool upRightWalled   = (upRight && upRight->hwall) || (gridCase.vwall) || upWalled || (up && up->vwall);
    bool downWalled      = gridCase.hwall;
    bool downLeftWalled  = gridCase.hwall || (left  && (left->hwall || left->vwall)) || (downLeft && downLeft->vwall);
    bool downRightWalled = gridCase.hwall || gridCase.vwall || (right && right->hwall) || (down && down->vwall);
    bool leftWalled      = left && left->vwall;
    bool rightWalled     = gridCase.vwall;

    if (!upLeftWalled    && isAvailable(upLeft) && !(!isAvailable(left) && !isAvailable(up)))
      gridCase.connectWith(upLeft);
    if (!upWalled        && isAvailable(up))
      gridCase.connectWith(up);
    if (!upRightWalled   && isAvailable(upRight) && !(!isAvailable(up) && !isAvailable(right)))
      gridCase.connectWith(upRight);
    if (!leftWalled      && isAvailable(left))
      gridCase.connectWith(left);
    if (!rightWalled     && isAvailable(right))
      gridCase.connectWith(right);
    if (!downLeftWalled  && isAvailable(downLeft) && !(!isAvailable(left) && !isAvailable(down)))
      gridCase.connectWith(downLeft);
    if (!downWalled      && isAvailable(down))
      gridCase.connectWith(down);
    if (!downRightWalled && isAvailable(downRight) && !(!isAvailable(down) && !isAvailable(right)))
      gridCase.connectWith(downRight);
  }
}

bool LevelGrid::hasPathfindingZones() const
{
  return getTilemap()->getPathfindindingZones().size() > 0;
}
