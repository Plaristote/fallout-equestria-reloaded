#pragma once
#include "../itemsrenderer.h"
#include "../../playervisibility.h"
#include "../../grid.h"
#include "../../lights.h"
#include "../../../leveltask.h"

class DynamicObjectsRenderer : public ItemsRendererInterface
{
  struct TrackedObject
  {
    qreal opacity = 0.0;
    bool  currentlyVisible = false;
    QMetaObject::Connection destroyedConnection;
  };

public:
  DynamicObjectsRenderer(TextureCache* textureCache, LevelTask* levelController)
      : ItemsRendererInterface(textureCache), m_levelGrid(levelController), m_playerVisibility(levelController), m_lighting(levelController)
  {
  }
  ~DynamicObjectsRenderer() override;

  void operator()(std::vector<RenderItem>& out, qreal elapsedMs) override;
  void updateVisibleObjects();

private:
  RenderItem renderSprite(Sprite* sprite, QPoint offset, int zKey, bool withDaylight);

  void forget(DynamicObject* object);
  void markVisible(DynamicObject* object);

  GridComponent*              m_levelGrid;
  PlayerVisibilityComponent*  m_playerVisibility;
  LightingComponent*          m_lighting;
  QHash<DynamicObject*, TrackedObject> m_tracked;
};
