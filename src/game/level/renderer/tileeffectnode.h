#pragma once
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QRectF>
#include <QPointF>
#include <QSizeF>
#include <QColor>
#include <QSGTexture>
#include "tileeffectmaterial.h"

class TileEffectNode : public QSGGeometryNode
{
public:
  TileEffectNode();
  ~TileEffectNode() override = default;

  void setTexture(QSGTexture *texture);
  void updateGeometry(const QRectF &rect, const QRectF &sourceRectPx, const QSize &textureSize);
  void setCropCircle(bool enabled, const QPointF &centerPointLocalPx, qreal radius, qreal smoothEdge);
  void setDaylight(bool enabled, const QColor &ambientColor, QSGTexture *maskTexture, const QPointF &maskOriginPx, const QSizeF &maskSizePx);

private:
  QSGGeometry*        m_geometry;
  TileEffectMaterial* m_material;
};
