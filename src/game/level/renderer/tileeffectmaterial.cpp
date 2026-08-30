#include "tileeffectmaterial.h"
#include <QMatrix4x4>
#include <cstring>

// -- Map for updateUniformData --
// offset  size  field
// 0       64    mat4  qt_Matrix
// 64      4     float qt_Opacity
// 72      8     vec2  texSize        (padded to 8-byte alignment)
// 80      8     vec2  centerPoint
// 88      4     float radius
// 92      4     float smoothEdge
// 96      4     bool  withClipping   (stored as int)
// 100     4     bool  withDaylight   (stored as int)
// 112     16    vec4  ambientColor   (padded to 16-byte alignment)
// 128     8     vec2  maskOrigin
// 136     8     vec2  maskSize
// 144     8     vec2  clipOrigin

constexpr int shaderUniformBufferSize = 160;

static void writeVec2(char *base, int offset, qreal x, qreal y)
{
  const float v[2] = { float(x), float(y) };
  std::memcpy(base + offset, v, sizeof(v));
}

static int comparePointF(const QPointF &a, const QPointF &b)
{
  if (a.x() != b.x())
    return a.x() < b.x() ? -1 : 1;
  if (a.y() != b.y())
    return a.y() < b.y() ? -1 : 1;
  return 0;
}

TileEffectMaterial::TileEffectMaterial()
{
  setFlag(QSGMaterial::Blending);
}

QSGMaterialType *TileEffectMaterial::type() const
{
  static QSGMaterialType staticType;
  return &staticType;
}

QSGMaterialShader *TileEffectMaterial::createShader(QSGRendererInterface::RenderMode) const
{
  return new TileEffectMaterialShader;
}

int TileEffectMaterial::compare(const QSGMaterial *other) const
{
  const auto *o = static_cast<const TileEffectMaterial *>(other);

  if (sourceTexture != o->sourceTexture)
    return sourceTexture < o->sourceTexture ? -1 : 1;
  if (maskTexture != o->maskTexture)
    return maskTexture < o->maskTexture ? -1 : 1;
  if (withClipping != o->withClipping)
    return withClipping ? -1 : 1;
  if (withDaylight != o->withDaylight)
    return withDaylight ? -1 : 1;
  if (int c = comparePointF(clipOrigin, o->clipOrigin); c != 0)
    return c;
  if (int c = comparePointF(centerPoint, o->centerPoint); c != 0)
    return c;
  if (radius != o->radius)
    return radius < o->radius ? -1 : 1;
  if (smoothEdge != o->smoothEdge)
    return smoothEdge < o->smoothEdge ? -1 : 1;
  if (ambientColor.rgba() != o->ambientColor.rgba())
    return ambientColor.rgba() < o->ambientColor.rgba() ? -1 : 1;
  if (int c = comparePointF(maskOrigin, o->maskOrigin); c != 0)
    return c;
  if (maskSize.width() != o->maskSize.width())
    return maskSize.width() < o->maskSize.width() ? -1 : 1;
  if (maskSize.height() != o->maskSize.height())
    return maskSize.height() < o->maskSize.height() ? -1 : 1;
  return 0;
}

TileEffectMaterialShader::TileEffectMaterialShader()
{
  setShaderFileName(VertexStage,   QLatin1String(":/game/level/renderer/tile.shader.vert.qsb"));
  setShaderFileName(FragmentStage, QLatin1String(":/game/level/renderer/tile.shader.frag.qsb"));
}

bool TileEffectMaterialShader::updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *)
{
  QByteArray *buf = state.uniformData();
  if (buf->size() < shaderUniformBufferSize)
    buf->resize(shaderUniformBufferSize);
  char *base = buf->data();

  auto *material = static_cast<TileEffectMaterial *>(newMaterial);

  if (state.isMatrixDirty())
  {
    const QMatrix4x4 m = state.combinedMatrix();
    std::memcpy(base + 0, m.constData(), 64);
  }
  if (state.isOpacityDirty())
  {
    const float opacity = state.opacity();
    std::memcpy(base + 64, &opacity, 4);
  }

  const QSize texSize = material->sourceTexture ? material->sourceTexture->textureSize() : QSize(1, 1);
  writeVec2(base, 72, texSize.width(), texSize.height());
  writeVec2(base, 80, material->centerPoint.x(), material->centerPoint.y());

  const float radius = float(material->radius);
  const float smoothEdge = float(material->smoothEdge);
  std::memcpy(base + 88, &radius, 4);
  std::memcpy(base + 92, &smoothEdge, 4);

  const qint32 withClipping = material->withClipping ? 1 : 0;
  const qint32 withDaylight = material->withDaylight ? 1 : 0;
  std::memcpy(base + 96, &withClipping, 4);
  std::memcpy(base + 100, &withDaylight, 4);

  const float ambient[4] = {
    float(material->ambientColor.redF()),
    float(material->ambientColor.greenF()),
    float(material->ambientColor.blueF()),
    float(material->ambientColor.alphaF()),
  };
  std::memcpy(base + 112, ambient, 16);

  writeVec2(base, 128, material->maskOrigin.x(), material->maskOrigin.y());
  writeVec2(base, 136, material->maskSize.width(), material->maskSize.height());
  writeVec2(base, 144, material->clipOrigin.x(), material->clipOrigin.y());

  return true;
}

void TileEffectMaterialShader::updateSampledImage(RenderState &state, int binding, QSGTexture **texture,
                                                   QSGMaterial *newMaterial, QSGMaterial *)
{
  auto *material = static_cast<TileEffectMaterial *>(newMaterial);

  if (binding == 1)
  {
    if (!material->sourceTexture)
      return;
    material->sourceTexture->setFiltering(QSGTexture::Linear);
    material->sourceTexture->commitTextureOperations(state.rhi(), state.resourceUpdateBatch());
    *texture = material->sourceTexture;
    return;
  }
  if (binding == 2)
  {
    if (!material->maskTexture)
      return;
    material->maskTexture->setFiltering(QSGTexture::Linear);
    material->maskTexture->commitTextureOperations(state.rhi(), state.resourceUpdateBatch());
    *texture = material->maskTexture;
    return;
  }
}
