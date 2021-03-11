#include "worldmapzone.h"

WorldMapZone::WorldMapZone(QObject *parent) : QObject(parent)
{

}

void WorldMapZone::addCase(QPoint position)
{
  if (cases.indexOf(position) < 0)
  {
    cases << position;
    emit casesChanged();
  }
}

void WorldMapZone::removeCase(QPoint position)
{
  auto index = cases.indexOf(position);

  if (index >= 0)
  {
    cases.removeAt(index);
    emit casesChanged();
  }
}
