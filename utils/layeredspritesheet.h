#ifndef LAYEREDSPRITESHEET_H
#define LAYEREDSPRITESHEET_H

#include <QImage>

class LayeredSpriteSheet : public QImage
{
public:
  LayeredSpriteSheet(QSize);

  void addLayer(const QString& group, const QString& animation);
  void addLayer(const QImage&);
};

#endif // LAYEREDSPRITESHEET_H
