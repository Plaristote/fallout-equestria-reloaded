#pragma once
#include <QHash>
#include <QSet>
#include <QImage>
#include <QSGTexture>
#include <QQuickWindow>
#include <QString>
#include <QDebug>
#include <memory>
#include <unordered_map>

class TextureCache
{
  typedef std::unordered_map<QString, std::unique_ptr<QSGTexture>> QSGTextureMap;
public:
  explicit TextureCache(QQuickWindow *window) : m_window(window)
  {
  }

  QSGTexture* texture(const QString& filePath);
  QSize       imageSize(const QString &filePath) const { return m_imageSizes.value(filePath); }
  QImage      image(const QString &filePath);
  void        clear();

private:
  QSGTexture* loadTexture(const QString&);

  QQuickWindow*          m_window;
  QSGTextureMap          m_textures;
  QHash<QString, QSize>  m_imageSizes;
  QHash<QString, QImage> m_images;
  QSet<QString>          m_failedPaths;
};
