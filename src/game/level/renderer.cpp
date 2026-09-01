#include <QSGSimpleTextureNode>
#include <QSGNode>
#include <QSGOpacityNode>
#include <QQuickWindow>
#include <QDebug>
#include <algorithm>
#include "renderer.h"
#include "../leveltask.h"
#include "renderer/tileeffectnode.h"
#include "renderer/items/roofs.h"
#include "renderer/items/walls.h"
#include "renderer/items/zones.h"
#include "renderer/items/ground.h"
#include "renderer/items/dynamicobjects.h"
#include "renderer/items/visualeffects.h"

LevelRenderer::LevelRenderer(QQuickItem *parent) : QQuickItem(parent)
{
  setFlag(QQuickItem::ItemHasContents, true);
  m_elapsedTimer.start();
}

void LevelRenderer::setLevelController(LevelTask *controller)
{
  if (m_levelController == controller)
    return;
  for (const QMetaObject::Connection &c : m_controllerConnections)
    disconnect(c);
  m_controllerConnections.clear();
  m_levelController = controller;
  if (m_levelController)
  {
    m_controllerConnections
        << connect(m_levelController, &QObject::destroyed, this, [this]{ setLevelController(nullptr); })
        << connect(m_levelController, &LevelTask::updated,                                  this, &LevelRenderer::update)
        << connect(m_levelController, &GridComponent::floorChanged,                         this, &LevelRenderer::initializeTilemap)
        << connect(m_levelController, &PlayerVisibilityComponent::visibleObjectsChanged,    this, [this]{ if (m_objectsRenderer) m_objectsRenderer->updateVisibleObjects(); })
        << connect(m_levelController, &PlayerVisibilityComponent::visibleCharactersChanged, this, [this]{ if (m_objectsRenderer) m_objectsRenderer->updateVisibleObjects(); });
    initializeItemsRenderers();
  }
  else
  {
    m_itemsRenderers.clear();
    m_daylightMask.reset();
    m_objectsRenderer = nullptr;
    m_zonesRenderer = nullptr;
    m_wallsRenderer = nullptr;
    m_roofsRenderer = nullptr;
  }
  emit levelControllerChanged();
}

void LevelRenderer::initializeItemsRenderers()
{
  m_itemsRenderers.clear();
  if (m_textureCache && m_levelController)
  {
    auto zonesRenderer   = std::make_unique<ZonesRenderer>(m_textureCache.get());
    auto objectsRenderer = std::make_unique<DynamicObjectsRenderer>(m_textureCache.get(), m_levelController);
    auto wallsRenderer   = std::make_unique<WallsRenderer>(m_textureCache.get(), m_levelController);
    auto roofsRenderer   = std::make_unique<RoofsRenderer>(m_levelController->getPreRenderPath(), m_textureCache.get());

    objectsRenderer->updateVisibleObjects();
    m_zonesRenderer   = zonesRenderer.get();
    m_objectsRenderer = objectsRenderer.get();
    m_wallsRenderer   = wallsRenderer.get();
    m_roofsRenderer   = roofsRenderer.get();
    m_itemsRenderers.push_back(std::make_unique<GroundRenderer>(m_levelController->getPreRenderPath(), m_textureCache.get()));
    m_itemsRenderers.push_back(std::move(zonesRenderer));
    m_itemsRenderers.push_back(std::move(wallsRenderer));
    m_itemsRenderers.push_back(std::move(objectsRenderer));
    m_itemsRenderers.push_back(std::make_unique<VisualEffectsRenderer>(m_textureCache.get(), m_levelController));
    m_itemsRenderers.push_back(std::move(roofsRenderer));
    for (auto& itemRenderer : m_itemsRenderers)
      itemRenderer->setWorldShiftX(m_originX);
    m_daylightMask = std::make_unique<DaylightMaskRenderer>(m_textureCache.get(), m_levelController);
    initializeTilemap();
  }
}

void LevelRenderer::initializeTilemap()
{
  for (auto& itemsRenderer : m_itemsRenderers)
    itemsRenderer->setTilemap(m_levelController->getTileMap(), m_levelController->getCurrentFloor());
  if (m_daylightMask)
    m_daylightMask->setTilemap(m_levelController->getTileMap(), m_levelController->getCurrentFloor());
  update();
}

bool LevelRenderer::renderWalls() const
{
  return m_wallsRenderer && m_wallsRenderer->isEnabled();
}

bool LevelRenderer::renderRoofs() const
{
  return m_roofsRenderer && m_roofsRenderer->isEnabled();
}

void LevelRenderer::setRenderWalls(bool enabled)
{
  qDebug() << "LevelRenderer::setRenderWalls" << enabled;
  if (!m_wallsRenderer || renderWalls() == enabled)
    return;
  m_wallsRenderer->setEnabled(enabled);
  emit renderWallsChanged();
  update();
  qDebug() << "LevelRenderer::setRenderWalls updated" << m_wallsRenderer->isEnabled();
}

void LevelRenderer::setRenderRoofs(bool enabled)
{
  qDebug() << "LevelRenderer::setRenderRoofs" << enabled;
  if (!m_roofsRenderer || renderRoofs() == enabled)
    return;
  m_roofsRenderer->setEnabled(enabled);
  emit renderRoofsChanged();
  update();
  qDebug() << "LevelRenderer::setRenderRoofs updated" << m_roofsRenderer->isEnabled();
}

void LevelRenderer::setOriginX(qreal value)
{
  if (qFuzzyCompare(m_originX, value))
    return;
  m_originX = value;
  for (auto& itemRenderer : m_itemsRenderers)
    itemRenderer->setWorldShiftX(m_originX);
  emit originXChanged();
  update();
}

QVariantList LevelRenderer::visibleZones() const
{
  if (m_zonesRenderer)
    return m_zonesRenderer->visibleZones();
  return QVariantList();
}

void LevelRenderer::setVisibleZones(const QVariantList &zones)
{
  if (m_zonesRenderer)
    m_zonesRenderer->setVisibleZones(zones);
  emit visibleZonesChanged();
  update();
}

QPointF LevelRenderer::playerCropCircleCenter() const
{
  Character* player = m_levelController ? m_levelController->getPlayer() : nullptr;
  QPointF    offset(35 + m_originX, 10);

  return player
    ? player->getSpritePosition() + offset
    : QPointF();
}

void LevelRenderer::ensureDummyDarkMaskTexture()
{
  if (m_dummyDarkMaskTexture || !window())
    return;
  QImage image(1, 1, QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  m_dummyDarkMaskTexture.reset(window()->createTextureFromImage(image));
}

// ---------------------------------------------------------------------------

static void appendQSGNode(QSGNode* root, QSGNode* node, qreal opacity)
{
  if (opacity >= 1.0)
    root->appendChildNode(node);
  else
  {
    auto *opacityNode = new QSGOpacityNode();
    opacityNode->setOpacity(opacity);
    opacityNode->appendChildNode(node);
    root->appendChildNode(opacityNode);
  }
}

void LevelRenderer::syncNodeTree(QSGNode *root, const std::vector<RenderItem> &sortedItems)
{
  // Most optimal way would be some sort of diff. In theory.
  // But we'll rebuild the tree at each frame. Works in practice.
  const bool    cropCircleGloballyEnabled = LevelTask::withPlayerCropCircle && m_levelController->getPlayer();
  const bool    daylightGloballyEnabled   = m_levelController->usesAmbientLight();
  const QPointF playerAnchor              = cropCircleGloballyEnabled ? playerCropCircleCenter() : QPointF();
  const QColor  tintColor                 = daylightGloballyEnabled ? m_levelController->getAmbientColor() : QColor();

  QSGTexture *maskTexture = m_daylightMask ? m_daylightMask->texture() : nullptr;
  const QSizeF maskSize   = m_daylightMask ? m_daylightMask->rect().size() : QSizeF();

  if (daylightGloballyEnabled)
    ensureDummyDarkMaskTexture();
  for (const RenderItem& item : sortedItems)
  {
    const ShaderPass passes = item.requiredPasses(daylightGloballyEnabled, cropCircleGloballyEnabled);

    if (passes == ShaderPass::None)
    {
      auto *node = new QSGSimpleTextureNode();
      node->setTexture(item.texture);
      node->setRect(item.destRect);
      node->setSourceRect(item.sourceRectPx);
      appendQSGNode(root, node, item.opacity);
    }
    else
    {
      auto *node = new TileEffectNode();
      node->setTexture(item.texture);
      node->updateGeometry(item.destRect, item.sourceRectPx, item.texture->textureSize());

      // PlayerCropCircle
      if (hasPass(passes, ShaderPass::CropCircleOnly))
      {
        const QPointF centerLocal = playerAnchor - item.destRect.topLeft();
        node->setCropCircle(true, centerLocal, cropCircleRadius, cropCircleSmoothEdge);
      }
      else
      {
        node->setCropCircle(false, QPointF(), 0, 0);
      }

      // AmbientLight
      if (hasPass(passes, ShaderPass::DaylightOnly))
      {
        if (item.usesDaylightMask && maskTexture)
          node->setDaylight(true, tintColor, maskTexture, item.maskLocalOrigin, maskSize);
        else
          node->setDaylight(true, tintColor, m_dummyDarkMaskTexture.get(), QPointF(0, 0), QSizeF(1, 1));
      }
      else
      {
        node->setDaylight(false, QColor(), nullptr, QPointF(), QSizeF());
      }
      appendQSGNode(root, node, item.opacity);
    }
  }
}

bool LevelRenderer::readyToRumble()
{
  if (window())
  {
    m_textureCache = std::make_unique<TextureCache>(window());
    initializeItemsRenderers();
    return true;
  }
  return false;
}

QSGNode* LevelRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
  QSGNode* root = oldNode ? oldNode : new QSGNode();
  const qreal elapsedMs = m_elapsedTimer.restart();
  std::vector<RenderItem> items;
  std::size_t count = 0;

  while (QSGNode* child = root->firstChild())
  {
    root->removeChildNode(child);
    delete child;
    count++;
  }
  items.reserve(count);
  if (m_levelController && (m_textureCache || readyToRumble())) [[likely]]
  {
    if (m_daylightMask)
      m_daylightMask->update(window());
    for (auto& itemsRenderer : m_itemsRenderers)
      itemsRenderer->run(items, elapsedMs);

    // stable_sort -> ties must preserve insertion order (maybe ? std::sort is faster, test it sometimes)
    std::stable_sort(items.begin(), items.end(),
                      [](const RenderItem &a, const RenderItem &b) { return a.zKey < b.zKey; });
    syncNodeTree(root, items);
  }
  return root;
}
