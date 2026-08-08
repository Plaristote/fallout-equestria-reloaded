#include "../pathfinding/levelgrid.h"
#include "../dynamicobject.h"
#include "../../game.h"
#include <cmath>
#include <QLineF>
#include <QRectF>
#include <QDebug>

bool lineIntersectsRect(QLineF line, QRectF rect);

void LevelGrid::eachCaseBetween(QPoint from, QPoint to, std::function<bool (const LevelGrid::CaseContent&)> callback) const
{
  const qreal   caseSize = 10;
  const qreal   caseCenter = 5;
  const int     minX = std::min(from.x(), to.x()), minY = std::min(from.y(), to.y());
  const int     maxX = std::max(from.x(), to.x()), maxY = std::max(from.y(), to.y());
  const QPointF sightFrom(
    static_cast<qreal>(from.x() - minX) * caseSize + caseCenter,
    static_cast<qreal>(from.y() - minY) * caseSize + caseCenter
  );
  const QPointF sightTo(
    static_cast<qreal>(to.x() - minX) * caseSize + caseCenter,
    static_cast<qreal>(to.y() - minY) * caseSize + caseCenter
  );
  const QLineF  sightLine(sightFrom, sightTo);
  int   cover = 0;
  int   obstacleCount = 0;

  for (int x = minX ; x <= maxX ; ++x)
  {
    for (int y = minY ; y <= maxY ; ++y)
    {
      const LevelGrid::CaseContent* gridCase;
      qreal posX, posY;
      QRectF caseRect;

      gridCase = const_cast<LevelGrid*>(this)->getGridCase(x, y);
      if (!gridCase)
        continue ;
      posX = static_cast<qreal>(x - minX) * 10;
      posY = static_cast<qreal>(y - minY) * 10;
      caseRect = QRectF(posX, posY, caseSize, caseSize);
      if (lineIntersectsRect(sightLine, caseRect))
      {
        if (!callback(*gridCase))
          return ;
      }
    }
  }
}

QVector<DynamicObject*> LevelGrid::getCharactersBetween(QPoint from, QPoint to) const
{
  QVector<DynamicObject*> list;

  eachCaseBetween(from, to, [&list](const LevelGrid::CaseContent& gridCase) -> bool
  {
    if (gridCase.occupant && gridCase.occupant->isCharacter())
      list.push_back(gridCase.occupant);
    return true;
  });
  std::sort(list.begin(), list.end(), [from](const DynamicObject* objectA, const DynamicObject* objectB)
  {
    return objectA->getDistance(from) < objectB->getDistance(from);
  });
  return list;
}
