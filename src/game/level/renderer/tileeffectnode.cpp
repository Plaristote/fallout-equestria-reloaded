#include "tileeffectnode.h"

TileEffectNode::TileEffectNode()
  : m_geometry(new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4))
  , m_material(new TileEffectMaterial)
{
  m_geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
  QSGGeometryNode::setGeometry(m_geometry);
  setMaterial(m_material);
  setFlag(QSGNode::OwnsGeometry, true);
  setFlag(QSGNode::OwnsMaterial, true);
}

void TileEffectNode::updateGeometry(const QRectF &rect, const QRectF &sourceRectPx, const QSize &textureSize)
{
  QRectF uv(0, 0, 1, 1);
  if (textureSize.width() > 0 && textureSize.height() > 0)
  {
    uv = QRectF(sourceRectPx.x() / textureSize.width(),
                sourceRectPx.y() / textureSize.height(),
                sourceRectPx.width() / textureSize.width(),
                sourceRectPx.height() / textureSize.height());
  }
  QSGGeometry::updateTexturedRectGeometry(m_geometry, rect, uv);
  markDirty(QSGNode::DirtyGeometry);

  m_material->clipOrigin = sourceRectPx.topLeft();
  markDirty(QSGNode::DirtyMaterial);
}

void TileEffectNode::setTexture(QSGTexture *texture)
{
  m_material->sourceTexture = texture;
  markDirty(QSGNode::DirtyMaterial);
}

void TileEffectNode::setCropCircle(bool enabled, const QPointF &centerPointLocalPx, qreal radius, qreal smoothEdge)
{
  m_material->withClipping = enabled;
  m_material->centerPoint = centerPointLocalPx;
  m_material->radius = radius;
  m_material->smoothEdge = smoothEdge;
  markDirty(QSGNode::DirtyMaterial);
}

void TileEffectNode::setDaylight(bool enabled, const QColor &ambientColor, QSGTexture *maskTexture,
                                  const QPointF &maskOriginPx, const QSizeF &maskSizePx)
{
  m_material->withDaylight = enabled;
  m_material->ambientColor = ambientColor;
  m_material->maskTexture = maskTexture;
  m_material->maskOrigin = maskOriginPx;
  m_material->maskSize = maskSizePx;
  markDirty(QSGNode::DirtyMaterial);
}
