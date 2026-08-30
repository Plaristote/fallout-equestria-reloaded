#include "texturecache.h"

void TextureCache::clear()
{
  m_textures.clear();
  m_imageSizes.clear();
  m_images.clear();
  m_failedPaths.clear();
}

QSGTexture* TextureCache::texture(const QString& filePath)
{
  if (auto it = m_textures.find(filePath); it != m_textures.end())
    return it->second.get();
  if (m_failedPaths.contains(filePath))
    return nullptr;
  return loadTexture(filePath);
}

QSGTexture* TextureCache::loadTexture(const QString& filePath)
{
  QImage image(filePath);
  std::unique_ptr<QSGTexture> texture;
  QSGTexture* raw;

  if (image.isNull())
  {
    qWarning() << "TextureCache: failed to load image:" << filePath;
    m_failedPaths.insert(filePath);
    return nullptr;
  }
  raw = m_window->createTextureFromImage(image);
  texture.reset(raw);
  m_textures.emplace(filePath, std::move(texture));
  m_imageSizes.insert(filePath, image.size());
  m_images.insert(filePath, image);
  return raw;
}

QImage TextureCache::image(const QString& filePath)
{
  if (auto it = m_images.find(filePath); it != m_images.end())
    return it.value();
  texture(filePath);
  return m_images.value(filePath);
}

