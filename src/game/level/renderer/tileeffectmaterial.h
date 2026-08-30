#pragma once
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QSGTexture>

class TileEffectMaterial : public QSGMaterial
{
public:
  TileEffectMaterial();

  QSGMaterialType*   type() const override;
  QSGMaterialShader* createShader(QSGRendererInterface::RenderMode) const override;
  int                compare(const QSGMaterial*) const override;

  QSGTexture* sourceTexture = nullptr;
  QPointF     clipOrigin;

  // PlayerCropCircle
  bool        withClipping = false;
  QPointF     centerPoint;   // in this sprite's local pixel space
  qreal       radius     = 0;
  qreal       smoothEdge = 0;

  // AmbientLight
  bool        withDaylight = false;
  QColor      ambientColor;
  QSGTexture* maskTexture = nullptr;
  QPointF     maskOrigin; // this sprite's position within maskTexture, in pixels
  QSizeF      maskSize; // in pixels
};

class TileEffectMaterialShader : public QSGMaterialShader
{
public:
  TileEffectMaterialShader();

  bool updateUniformData(RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override;
  void updateSampledImage(RenderState& state, int binding, QSGTexture** texture, QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override;
};
