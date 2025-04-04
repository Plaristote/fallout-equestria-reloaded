#include "tilemask.h"

TileMask::TileMask(QObject *parent) : QObject(parent)
{
}

bool TileMask::isInside(int x, int y, int) const
{
  return isInside(x, y);
}
