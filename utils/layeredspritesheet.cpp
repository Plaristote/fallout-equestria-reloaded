#include "layeredspritesheet.h"
#include <QPainter>

LayeredSpriteSheet::LayeredSpriteSheet(QSize size) : QImage(size, QImage::Format_ARGB32)
{
  fill(Qt::transparent);
}

void LayeredSpriteSheet::addLayer(const QImage& source)
{
  QPainter painter;

  painter.begin(this);
  painter.drawImage(0, 0, source);
  painter.end();
}

void LayeredSpriteSheet::addColorLayer(QColor color, const QImage &mask)
{
  if (color != Qt::transparent)
  {
    QImage   colorLayer(size(), QImage::Format_ARGB32);
    QPainter painter;

    colorLayer.fill(Qt::transparent);
    painter.begin(&colorLayer);
    for (int x = 0 ; x < size().width() ; ++x)
    {
      for (int y = 0 ; y < size().height() ; ++y)
      {
         QColor baseColor   = mask.pixelColor(x, y);
         bool   blackish    = baseColor.red() == 0 && baseColor.green() == 0 && baseColor.blue() == 0;

         if (!blackish && baseColor.alpha() != QColor(Qt::transparent).alpha())
           colorLayer.setPixelColor(x, y, color);
      }
    }
    painter.end();
    painter.begin(this);
    painter.drawImage(0, 0, colorLayer);
    painter.end();
  }
}
