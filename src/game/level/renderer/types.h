#pragma once
#include <QRectF>
#include <QPointF>
#include <QImage>
#include <QSGTexture>
#include <cstdint>

enum struct ShaderPass : uint8_t
{
  None            = 0,
  CropCircleOnly  = 1 << 0,
  DaylightOnly    = 1 << 1,
  CropAndDaylight = CropCircleOnly | DaylightOnly,
};

inline ShaderPass operator|(ShaderPass a, ShaderPass b)
{
  return static_cast<ShaderPass>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline bool hasPass(ShaderPass v, ShaderPass flag)
{
  return (static_cast<uint8_t>(v) & static_cast<uint8_t>(flag)) != 0;
}

enum struct RenderLayer : uint8_t
{
  Ground,
  Zone,
  Light,
  Object,
  Character,
  Wall,
  Effect,
  Roof,
  Cursor,
};

struct RenderItem
{
  RenderLayer layer = RenderLayer::Ground;
  QSGTexture* texture = nullptr;
  QRectF      destRect;
  QRectF      sourceRectPx;
  qreal       opacity = 1.0;
  int64_t     zKey  = 0;
  bool        excludedFromDaylight = false;
  bool        usesDaylightMask = false;
  bool        eligibleForCropCircle = false;

  QPointF maskLocalOrigin; // position within the shared daylight mask (worldPosition - groundRect.topLeft())

  ShaderPass requiredPasses(bool globalDaylightEnabled, bool globalCropCircleEnabled) const
  {
    ShaderPass p = ShaderPass::None;
    if (globalCropCircleEnabled && eligibleForCropCircle)
      p = p | ShaderPass::CropCircleOnly;
    if (globalDaylightEnabled && (usesDaylightMask || !excludedFromDaylight))
      p = p | ShaderPass::DaylightOnly;
    return p;
  }
};
