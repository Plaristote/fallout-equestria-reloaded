#include "visualeffects.h"
#include "../zindex.h"
#include <algorithm>

void VisualEffectsRenderer::operator()(std::vector<RenderItem>& out, qreal)
{
  for (Sprite* sprite : m_controller->getVisualEffects())
    {
      const QPoint pos = sprite->getSpritePosition();
      const QRect  clip = sprite->getClippedRect();
      const QPoint gridPosition = m_controller->getTilePosition(pos);
      const int    tileX = std::clamp<int>(gridPosition.x(), 0, m_mapSize.width() - 1);
      const int    tileY = std::clamp<int>(gridPosition.y(), 0, m_mapSize.height() - 1);
      RenderItem   item;

      item.layer                = RenderLayer::Effect;
      item.texture              = m_textureCache.texture(sprite->getSpriteSource());
      item.zKey                 = ZOrder::forObject(tileX, tileY, m_mapSize.width(), 3);
      item.destRect             = QRectF(pos + m_worldShift, clip.size());
      item.sourceRectPx         = clip;
      item.excludedFromDaylight = true;
      if (item.texture)
        out.push_back(item);
    }
}
