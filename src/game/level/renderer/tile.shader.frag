#version 440

layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf
{
  mat4  qt_Matrix;
  float qt_Opacity;

  vec2  texSize;       // source texture size, in pixels

  // PlayerCropCircle
  vec2  centerPoint;   // circle center, in THIS sprite's local pixel space
  float radius;        // circle radius, in pixels
  float smoothEdge;    // falloff width, in pixels
  bool  withClipping;  // apply the cutout at all

  // DaylightTint
  bool  withDaylight;  // apply the tint pass at all
  vec4  ambientColor;  // tint color (rgb) and strength (a, 0..1)

  // Where this sprite sits within the shared per-floor daylight mask
  // (LevelRenderer::daylightMaskTexture()), in the mask's own pixel
  // space. For objects/characters that don't spatially mask (they're
  // either fully exempt -- never reach this shader -- or fully tinted),
  // LevelRenderer binds a 1x1 "always dark" dummy texture here instead,
  // so this same code path applies full tint without a shader branch.
  vec2  maskOrigin;
  vec2  maskSize;

  // This sprite's own clip rect top-left within `source`'s full pixel
  // space. `coord * texSize` recovers a position in that same full-image
  // space (since coord is UV across the whole shared tileset, not just
  // this sprite's own clip), so it has to be subtracted before comparing
  // against centerPoint, which is expressed relative to this sprite's own
  // local origin. Without this, the crop circle's effective position
  // drifts by whatever offset this sprite's clip happens to sit at within
  // its tileset -- close to correct for some sprites, wildly off for
  // others, depending on layout.
  vec2  clipOrigin;
};

layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D daylightMask;

void main()
{
  vec4 texColor = texture(source, coord);

  float cropAlpha = 1.0;
  if (withClipping)
  {
    vec2 pixelCoord = coord * texSize - clipOrigin;
    float dist = length(pixelCoord - centerPoint);
    // 0.0 (fully transparent) near the player, 1.0 (fully opaque)
    cropAlpha = smoothstep(radius - smoothEdge, radius, dist);
  }

  vec3 rgb = texColor.rgb;
  if (withDaylight)
  {
    vec2 maskCoord = (maskOrigin + coord * texSize - clipOrigin) / maskSize;
    // Mask alpha: 1 = this pixel is inside an active light layer
    // (already lit, skip the tint), 0 = apply full tint.
    float lit = texture(daylightMask, maskCoord).a;
    vec3 tinted = mix(rgb, rgb * ambientColor.rgb, ambientColor.a);
    rgb = mix(tinted, rgb, lit);
  }

  fragColor = vec4(rgb, texColor.a) * cropAlpha * qt_Opacity;
}
