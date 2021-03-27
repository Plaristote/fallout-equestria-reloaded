#include "worldmapcity.h"
#include <cmath>

WorldMapCity::WorldMapCity(QObject* parent) : QObject(parent)
{

}

bool WorldMapCity::isInside(QPoint point)
{
  auto a = position.x() - point.x();
  auto b = position.y() - point.y();

  auto patchedSize = size - 10; // for some reasons, drawn circle are smaller than they should be ?

  return std::sqrt(a * a + b * b) < patchedSize;
}
