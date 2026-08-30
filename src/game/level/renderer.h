#pragma once

#include <QQuickItem>
#include <QColor>
#include <QPointF>
#include <QHash>
#include <QVariant>
#include <QElapsedTimer>
#include <memory>
#include <vector>
#include "renderer/types.h"
#include "renderer/texturecache.h"
#include "renderer/itemsrenderer.h"
#include "renderer/daylightmask.h"

class QSGNode;
class LevelTask;
class DynamicObjectsRenderer;
class ZonesRenderer;

class LevelRenderer : public QQuickItem
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(LevelTask *levelController READ levelController WRITE setLevelController NOTIFY levelControllerChanged)
  Q_PROPERTY(bool renderWalls READ renderWalls WRITE setRenderWalls NOTIFY renderWallsChanged)
  Q_PROPERTY(bool renderRoofs READ renderRoofs WRITE setRenderRoofs NOTIFY renderRoofsChanged)
  Q_PROPERTY(qreal originX READ originX WRITE setOriginX NOTIFY originXChanged)
  Q_PROPERTY(QVariantList visibleZones READ visibleZones WRITE setVisibleZones NOTIFY visibleZonesChanged)

  typedef std::vector<std::unique_ptr<ItemsRendererInterface>> ItemsRenderers;
  static constexpr qreal characterWidth = 47;
  static constexpr qreal cropCircleRadius = characterWidth * 1.5;
  static constexpr qreal cropCircleSmoothEdge = cropCircleRadius * 0.4;

public:
  explicit LevelRenderer(QQuickItem *parent = nullptr);

  LevelTask*   levelController() const { return m_levelController; }
  void         setLevelController(LevelTask *controller);
  bool         renderWalls() const { return m_renderWalls; }
  void         setRenderWalls(bool enabled);
  bool         renderRoofs() const { return m_renderRoofs; }
  void         setRenderRoofs(bool enabled);
  qreal        originX() const { return m_originX; }
  void         setOriginX(qreal value);
  QVariantList visibleZones() const;
  void         setVisibleZones(const QVariantList &zones);
  QPointF      playerCropCircleCenter() const;

signals:
  void         levelControllerChanged();
  void         renderWallsChanged();
  void         renderRoofsChanged();
  void         originXChanged();
  void         visibleZonesChanged();

private:
  void         initializeItemsRenderers();
  void         initializeTilemap();
  void         ensureDummyDarkMaskTexture();
  bool         readyToRumble();
  QSGNode*     updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
  void         syncNodeTree(QSGNode *root, const std::vector<RenderItem> &sortedItems);

  LevelTask*                            m_levelController = nullptr;
  bool                                  m_renderWalls = true;
  bool                                  m_renderRoofs = true;
  qreal                                 m_originX = 0;
  QElapsedTimer                         m_elapsedTimer;
  std::unique_ptr<TextureCache>         m_textureCache; // lazily created in updatePaintNode/readyToRumble
  std::unique_ptr<QSGTexture>           m_dummyDarkMaskTexture;
  ItemsRenderers                        m_itemsRenderers;
  DynamicObjectsRenderer*               m_objectsRenderer = nullptr;
  ZonesRenderer*                        m_zonesRenderer = nullptr;
  std::unique_ptr<DaylightMaskRenderer> m_daylightMask;
  QVector<QMetaObject::Connection>      m_controllerConnections;
};
