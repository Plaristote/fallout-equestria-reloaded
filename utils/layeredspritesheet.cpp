#include "layeredspritesheet.h"
#include "game/animationlibrary.h"
#include <QPainter>

LayeredSpriteSheet::LayeredSpriteSheet(QSize size) : QImage(size, QImage::Format_ARGB32)
{
  fill(Qt::transparent);
}

void LayeredSpriteSheet::addLayer(const QString& group, const QString& animation)
{
  const QImage& image = AnimationLibrary::get()->getImage(group, animation);

  addLayer(image);
}

void LayeredSpriteSheet::addLayer(const QImage& source)
{
  QPainter painter;

  painter.begin(this);
  painter.drawImage(0, 0, source);
  painter.end();
}
