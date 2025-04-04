#include "camera.h"
#include "tilemap/tilemap.h"
#include <algorithm>

using namespace std;

static QPoint getCaseAt(QSize tileSize, int posX, int posY)
{
  const auto a = posY / tileSize.height();
  const auto b = posX / tileSize.width();
  return QPoint(a + b, a - b);
}

static QRect screenRect(QPoint offset, QSize size)
{
  return QRect(-offset.x(), -offset.y(), size.width(), size.height());
}

CameraComponent::CameraComponent(QObject* parent) : ParentType(parent)
{
  connect(this, &CameraComponent::cameraMoved,       &updateTimer, [this]() { updateTimer.start(); });
  connect(this, &CameraComponent::cameraSizeChanged, &updateTimer, [this]() { updateTimer.start(); });
  connect(&updateTimer, &QTimer::timeout, this, &CameraComponent::updateRenderedTiles);
  updateTimer.setInterval(50);
}

void CameraComponent::updateRenderedTiles()
{
  QRect newRenderedTiles = getRenderedTiles();

  if (newRenderedTiles != renderedTiles)
  {
    renderedTiles = newRenderedTiles;
    emit renderedTilesChanged();
  }
}

QRect CameraComponent::getRenderedTiles() const
{
  TileMap* tilemap = getTileMap();

  if (tilemap)
  {
    QSize       tileSize    = tilemap->getTileSize();
    QRect       rect        = screenRect(offset, size);
    QPoint      topLeft     = getCaseAt(tileSize, rect.left(),  rect.top());
    QPoint      topRight    = getCaseAt(tileSize, rect.right(), rect.top());
    QPoint      bottomRight = getCaseAt(tileSize, rect.right(), rect.bottom());
    QPoint      bottomLeft  = getCaseAt(tileSize, rect.left(),  rect.bottom());
    vector<int> xs{topLeft.x(), topRight.x(), bottomRight.x(), bottomLeft.x()};
    vector<int> ys{topLeft.y(), topRight.y(), bottomRight.y(), bottomLeft.y()};

    return QRect(
      QPoint(*min_element(xs.begin(), xs.end()), *min_element(ys.begin(), ys.end())),
      QPoint(*max_element(xs.begin(), xs.end()), *max_element(ys.begin(), ys.end()))
    );
  }
  return QRect();
}
