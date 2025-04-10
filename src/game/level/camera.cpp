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
  connect(this, &CameraComponent::renderedTilesChanged, this, &CameraComponent::updateCulledTiles);
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
    emit renderedTilesChanged(renderedTiles);
  }
}

QRect CameraComponent::getRenderedTiles() const
{
  TileMap* tilemap = getTileMap();

  if (tilemap)
  {
    const QSize       tileSize    = tilemap->getTileSize();
    const QRect       rect        = screenRect(offset, size);
    const QPoint      topLeft     = getCaseAt(tileSize, rect.left(),  rect.top());
    const QPoint      topRight    = getCaseAt(tileSize, rect.right(), rect.top());
    const QPoint      bottomRight = getCaseAt(tileSize, rect.right(), rect.bottom());
    const QPoint      bottomLeft  = getCaseAt(tileSize, rect.left(),  rect.bottom());
    const vector<int> xs{topLeft.x(), topRight.x(), bottomRight.x(), bottomLeft.x()};
    const vector<int> ys{topLeft.y(), topRight.y(), bottomRight.y(), bottomLeft.y()};

    return QRect(
      QPoint(
        *min_element(xs.begin(), xs.end()),
        *min_element(ys.begin(), ys.end())
      ),
      QPoint(
        *max_element(xs.begin(), xs.end()),
        *max_element(ys.begin(), ys.end())
      )
    );
  }
  return QRect();
}
