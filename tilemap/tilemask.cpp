#include "tilemask.h"

TileMask::TileMask(QObject *parent) : QObject(parent)
{
}

bool TileMask::isInside(int x, int y, unsigned char) const
{
  return isInside(x, y);
}
