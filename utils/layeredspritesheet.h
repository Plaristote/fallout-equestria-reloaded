#ifndef LAYEREDSPRITESHEET_H
#define LAYEREDSPRITESHEET_H

#include <QImage>

class LayeredSpriteSheet : public QImage
{
public:
  LayeredSpriteSheet(QSize);

  void addLayer(const QImage&);
  void addColorLayer(QColor, const QImage& mask);
};

#endif // LAYEREDSPRITESHEET_H
