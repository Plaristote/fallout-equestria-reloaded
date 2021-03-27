#include "textbubble.h"
#include "dynamicobject.h"

TextBubble::TextBubble(QObject* parent, DynamicObject* target) : QObject(parent), target(target)
{
}

void TextBubble::initialize(const QString& content, qint64 duration, const QString& color)
{
  this->content  = content;
  this->color    = color;
  this->timeLeft = duration;
  emit contentChanged();
  emit colorChanged();
  emit opacityChanged();
}

bool TextBubble::update(qint64 delta)
{
  QPoint newPosition = target->getSpritePosition();

  timeLeft -= delta;
  if (newPosition != position)
  {
    position = newPosition;
    emit positionChanged();
  }
  if (timeLeft < 1000)
    emit opacityChanged();
  return timeLeft > 0;
}

float TextBubble::getOpacity() const
{
  if (timeLeft < 1000)
    return static_cast<float>(timeLeft) / 1000.f;
  return 1;
}
