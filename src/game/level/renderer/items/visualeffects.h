#pragma once
#include "../itemsrenderer.h"
#include "game/level/visualeffects.h"

class VisualEffectsRenderer : public ItemsRendererInterface
{
public:
  VisualEffectsRenderer(TextureCache* textureCache, VisualEffectsComponent* controller)
    : ItemsRendererInterface(textureCache), m_controller(controller)
  {
  }

  void operator()(std::vector<RenderItem>& out, qreal) override;

private:
  VisualEffectsComponent* m_controller;
};
