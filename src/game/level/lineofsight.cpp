#include "../pathfinding/levelgrid.h"
#include "../dynamicobject.h"
#include <cmath>
#include <QLineF>
#include <QRectF>
#include <QDebug>

bool lineIntersectsRect(QLineF line, QRectF rect)
{
  QLineF leftSide(rect.topLeft(), rect.bottomLeft());
  QLineF topSide(rect.topLeft(), rect.topRight());
  QLineF rightSide(rect.topRight(), rect.bottomRight());
  QLineF bottomSide(rect.bottomLeft(), rect.bottomRight());

  return (leftSide.intersects(line, nullptr)   == QLineF::BoundedIntersection) ||
         (topSide.intersects(line, nullptr)    == QLineF::BoundedIntersection) ||
         (rightSide.intersects(line, nullptr)  == QLineF::BoundedIntersection) ||
         (bottomSide.intersects(line, nullptr) == QLineF::BoundedIntersection);
}

static bool losIntersectsWithHWall(QLineF line, QRectF rect)
{
  QLineF bottomSide(rect.bottomLeft(), rect.bottomRight());

  return bottomSide.intersects(line, nullptr) == QLineF::BoundedIntersection;
}

static bool losIntersectsWithVWall(QLineF line, QRectF rect)
{
  QLineF rightSide(rect.topRight(), rect.bottomRight());

  return rightSide.intersects(line, nullptr) == QLineF::BoundedIntersection;
}

static QPair<char, char> getCoverThroughCase(const LevelGrid::CaseContent* gridCase, const QLineF sightLine, const QRectF caseRect, const QPoint from, const QPoint target)
{
  bool isFrom   = gridCase->position == from;
  bool isTarget = gridCase->position == target;
  char cover = 0;
  char obstacleCount = 0;

  if (gridCase->block)
    obstacleCount++;
  if (gridCase->hwall && losIntersectsWithHWall(sightLine, caseRect))
  {
    cover = std::max(cover, gridCase->hcover);
    obstacleCount++;
  }
  if (gridCase->vwall && losIntersectsWithVWall(sightLine, caseRect))
  {
    cover = std::max(cover, gridCase->vcover);
    obstacleCount++;
  }
  if (!isFrom && !isTarget)
  {
    cover = std::max(cover, gridCase->cover);
    if (gridCase->occupant)
    {
      cover = std::max(cover, static_cast<char>(gridCase->occupant->getCoverValue()));
      obstacleCount++;
    }
  }
  return {cover, obstacleCount};
}

static bool ignoreCover(int fromX, int fromY, int caseX, int caseY)
{
  return std::abs(fromX - caseX) <= 1 && std::abs(fromY - caseY) <= 1;
}

int LevelGrid::getVisionQuality(int fromX, int fromY, int toX, int toY)
{
  const qreal   caseSize = 10;
  const qreal   caseCenter = 5;
  const int     minX = std::min(fromX, toX), minY = std::min(fromY, toY);
  const int     maxX = std::max(fromX, toX), maxY = std::max(fromY, toY);
  const QPointF sightFrom(
    static_cast<qreal>(fromX - minX) * caseSize + caseCenter,
    static_cast<qreal>(fromY - minY) * caseSize + caseCenter
  );
  const QPointF sightTo(
    static_cast<qreal>(toX - minX) * caseSize + caseCenter,
    static_cast<qreal>(toY - minY) * caseSize + caseCenter
  );
  const QLineF  sightLine(sightFrom, sightTo);
  int   cover = 0;
  int   obstacleCount = 0;

  /* Initial implementation
  for (int x = minX ; x <= maxX ; ++x)
  {
    for (int y = minY ; y <= maxY ; ++y)
    {
      LevelGrid::CaseContent* gridCase;
      qreal posX, posY;
      QRectF caseRect;

      gridCase = getGridCase(x, y);
      if (!gridCase || (!gridCase->occupant && (gridCase->cover + gridCase->hcover + gridCase->vcover) == 0))
        continue ;
      posX = static_cast<qreal>(x - minX) * 10;
      posY = static_cast<qreal>(y - minY) * 10;
      caseRect = QRectF(posX, posY, caseSize, caseSize);
      if (lineIntersectsRect(sightLine, caseRect))
      {
        auto result = getCoverThroughCase(gridCase, sightLine, caseRect, {fromX, fromY}, {toX, toY});

        if (result.first >= 100)
          return 0;
        else if (!ignoreCover(fromX, fromY, gridCase->position.x, gridCase->position.y))
        {
          cover = std::max<int>(cover, result.first);
          obstacleCount += result.second;
        }
      }
    }
  }
  */

  ///* Claude implementation
  {
    int x = fromX, y = fromY;
    int dx = std::abs(toX - fromX), dy = std::abs(toY - fromY);
    int sx = (fromX < toX) ? 1 : -1;
    int sy = (fromY < toY) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        LevelGrid::CaseContent* gridCase = getGridCase(x, y);
        if (gridCase)
        {
            qreal posX = static_cast<qreal>(x - minX) * caseSize;
            qreal posY = static_cast<qreal>(y - minY) * caseSize;
            QRectF caseRect(posX, posY, caseSize, caseSize);
            auto result = getCoverThroughCase(gridCase, sightLine, caseRect, {fromX, fromY}, {toX, toY});

            if (result.first >= 100)
                return 0;
            else if (!ignoreCover(fromX, fromY, x, y))
            {
                cover = std::max<int>(cover, result.first);
                obstacleCount += result.second;
            }
        }

        if (x == toX && y == toY)
            break;

        int e2 = 2 * err;
        bool stepX = (e2 > -dy);
        bool stepY = (e2 <  dx);

        // When stepping diagonally, also check the two axis-aligned neighbors
        // the line passes through — Bresenham skips them but the line clips them
        if (stepX && stepY)
        {
            // Check horizontal neighbor before diagonal step
            LevelGrid::CaseContent* hCase = getGridCase(x + sx, y);
            if (hCase)
            {
                qreal posX = static_cast<qreal>(x + sx - minX) * caseSize;
                qreal posY = static_cast<qreal>(y - minY) * caseSize;
                QRectF caseRect(posX, posY, caseSize, caseSize);
                if (lineIntersectsRect(sightLine, caseRect))
                {
                    auto result = getCoverThroughCase(hCase, sightLine, caseRect, {fromX, fromY}, {toX, toY});
                    if (result.first >= 100) return 0;
                    else if (!ignoreCover(fromX, fromY, x + sx, y))
                    {
                        cover = std::max<int>(cover, result.first);
                        obstacleCount += result.second;
                    }
                }
            }
            // Check vertical neighbor before diagonal step
            LevelGrid::CaseContent* vCase = getGridCase(x, y + sy);
            if (vCase)
            {
                qreal posX = static_cast<qreal>(x - minX) * caseSize;
                qreal posY = static_cast<qreal>(y + sy - minY) * caseSize;
                QRectF caseRect(posX, posY, caseSize, caseSize);
                if (lineIntersectsRect(sightLine, caseRect))
                {
                    auto result = getCoverThroughCase(vCase, sightLine, caseRect, {fromX, fromY}, {toX, toY});
                    if (result.first >= 100) return 0;
                    else if (!ignoreCover(fromX, fromY, x, y + sy))
                    {
                        cover = std::max<int>(cover, result.first);
                        obstacleCount += result.second;
                    }
                }
            }
        }

        if (stepX) { err -= dy; x += sx; }
        if (stepY) { err += dx; y += sy; }
    }
  }
  //*/


  return std::max(0, 100 - (cover + obstacleCount));
}
