#pragma once
#include <climits>

// Z-order formula (from the old QML renderer):
//
//   walls:      z = (tx + ty * mapWidth) * 4 + (vwall||hwall ? 2 : 0)
//   objects:    z = (posX + posY * mapWidth) * 4 + zIndex - 1
//   roofs:      z = +infinity (always topmost, but below cursor)
//   cursor:     z = +infinity, drawn after roofs
//
// The 4* stride leaves two sub-slots per tile so that, within the same tile,
// walls (which sit "in front" of whatever's on the tile behind them) can be
// ordered relative to ground-level objects/characters occupying that same
// cell.

namespace ZOrder
{
  constexpr int stride = 4;
  constexpr int roofAndCursorBase = INT_MAX - 2;

  inline int tileBase(int tileX, int tileY, int mapWidth)
  {
    return (static_cast<int>(tileX) + static_cast<int>(tileY) * mapWidth) * stride;
  }

  inline int forWall(int tileX, int tileY, int mapWidth, bool hasVOrHWall)
  {
    return tileBase(tileX, tileY, mapWidth) + (hasVOrHWall ? 2 : 0);
  }

  inline int forObject(int posX, int posY, int mapWidth, int zIndex)
  {
    return tileBase(posX, posY, mapWidth) + zIndex - 1;
  }

  inline int forRoof()
  {
    return roofAndCursorBase;
  }
}
