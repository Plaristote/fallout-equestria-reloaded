#include "dynamicobjects.h"
#include "../zindex.h"
#include <algorithm>

DynamicObjectsRenderer::~DynamicObjectsRenderer()
{
  for (auto it = m_tracked.begin(); it != m_tracked.end(); ++it)
    QObject::disconnect(it->destroyedConnection);
}

void DynamicObjectsRenderer::forget(DynamicObject* object)
{
  m_tracked.remove(object);
}

void DynamicObjectsRenderer::markVisible(DynamicObject* object)
{
  auto it = m_tracked.find(object);

  if (it == m_tracked.end())
  {
    TrackedObject state;

    state.currentlyVisible = true;
    state.destroyedConnection = QObject::connect(object, &QObject::destroyed, [this, object] { forget(object); });
    m_tracked.insert(object, state);
  }
  else
  {
    it->currentlyVisible = true;
  }
}

void DynamicObjectsRenderer::updateVisibleObjects()
{
  for (auto it = m_tracked.begin(); it != m_tracked.end(); ++it)
    it->currentlyVisible = false;
  for (DynamicObject* object : m_playerVisibility->getVisibleObjects())
    markVisible(object);
  for (Character* character : m_playerVisibility->getVisibleCharacters())
    markVisible(character);
}

void DynamicObjectsRenderer::operator()(std::vector<RenderItem>& out, qreal elapsedMs)
{
  const qreal maxDelta = elapsedMs > 0 ? (elapsedMs / g_fadeDurationMs) : 1.0;

  for (auto it = m_tracked.begin() ; it != m_tracked.end() ;)
  {
    DynamicObject* object = it.key();
    TrackedObject& state  = it.value();
    const qreal    target = state.currentlyVisible ? 1.0 : 0.0;

    if (state.opacity != target)
    {
      state.opacity = state.opacity < target
        ? std::min(target, state.opacity + maxDelta)
        : std::max(target, state.opacity - maxDelta);
    }
    if (state.opacity <= 0.0 && !state.currentlyVisible)
    {
      QObject::disconnect(state.destroyedConnection);
      it = m_tracked.erase(it);
      continue ;
    }
    else if (state.opacity > 0.0)
    {
      QPoint gridPosition = object->getPosition();
      QPoint offset       = m_levelGrid->getAdjustedOffsetFor(object);
      int    zKey         = ZOrder::forObject(gridPosition.x(), gridPosition.y(), m_mapSize.width(), object->getZIndex());
      bool   withDaylight = !m_lighting->isLit(gridPosition);
      RenderItem item     = renderSprite(object, offset, zKey, withDaylight);

      item.opacity = state.opacity;
      out.push_back(item);
    }
    ++it;
  }
}

RenderItem DynamicObjectsRenderer::renderSprite(Sprite* sprite, QPoint offset, int zKey, bool withDaylight)
{
  QRect      clip = sprite->getClippedRect();
  RenderItem item;

  item.texture               = m_textureCache.texture(sprite->getSpriteSource());
  item.layer                 = RenderLayer::Object;
  item.zKey                  = zKey;
  item.destRect              = QRectF(offset + m_worldShift, clip.size());
  item.sourceRectPx          = clip;
  item.eligibleForCropCircle = false;
  item.excludedFromDaylight  = !withDaylight;
  return item;
}
