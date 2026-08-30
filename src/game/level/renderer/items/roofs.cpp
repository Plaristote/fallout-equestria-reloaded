#include "roofs.h"
#include "../zindex.h"

void RoofsRenderer::initialize()
{
  m_roofFadeStates.clear();
  for (TileLayer* roof : m_tilemap->getRoofs())
    m_roofFadeStates.insert(roof, roof->isVisible() ? 1.0 : 0.0);
}

void RoofsRenderer::setTilemap(TileMap* tilemap, int floor)
{
  ItemsRendererInterface::setTilemap(tilemap, floor);
  m_wallHeight   = m_tilemap->getTileSize().height() * 2;
  initialize();
}

void RoofsRenderer::operator()(std::vector<RenderItem>& out, qreal elapsedMs)
{
  const qreal maxDelta = elapsedMs > 0 ? (elapsedMs / g_fadeDurationMs) : 1.0;

  for (TileLayer* roof : m_tilemap->getRoofs())
  {
    if (updateOpacity(roof, maxDelta))
      render(out, roof);
  }
}

bool RoofsRenderer::updateOpacity(TileLayer* roof, const qreal maxDelta)
{
  const qreal target = roof->isVisible() ? 1.0 : 0.0;
  qreal&      state  = m_roofFadeStates[roof];

  if (state != target)
  {
    state = state < target
      ? std::min(target, state + maxDelta)
      : std::max(target, state - maxDelta);
  }
  return state > 0;
}

QString RoofsRenderer::prerenderPathFor(TileLayer* roof) const
{
  return QStringLiteral("%1floor%2_roof_%3.png")
    .arg(m_pathPrefix)
    .arg(m_currentFloor)
    .arg(roof->getName());
}

QPointF RoofsRenderer::worldPositionFor(TileLayer* roof) const
{
  const bool isFloor = roof->getName().startsWith(QStringLiteral("floor_"));
  const QRect renderRect = roof->getRenderedRect();

  if (isFloor)
  {
    return QPointF(
      renderRect.x() - renderRect.width() / 2.0,
      -m_wallHeight + m_wallHeight * m_currentFloor
    );
  }
  return QPointF(renderRect.x(), 0);
}

void RoofsRenderer::render(std::vector<RenderItem>& out, TileLayer* roof)
{
  RenderItem    item;
  const QRect   renderRect = roof->getRenderedRect();
  const QString path       = prerenderPathFor(roof);

  item.layer   = RenderLayer::Roof;
  item.texture = m_textureCache.texture(path);
  item.zKey    = ZOrder::forRoof();
  item.destRect.setTopLeft(worldPositionFor(roof) + m_worldShift);
  item.destRect.setSize(renderRect.size());
  item.sourceRectPx = QRectF(QPointF(0, 0), m_textureCache.imageSize(path));
  item.opacity = m_roofFadeStates[roof];
  item.eligibleForCropCircle = true;
  if (item.texture)
    out.push_back(item);
}
