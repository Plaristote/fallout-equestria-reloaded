#include "textbubbles.h"

TextBubblesComponent::TextBubblesComponent(QObject *parent) : InteractionComponent(parent)
{

}

void TextBubblesComponent::update(qint64 delta)
{
  auto oldLength = textBubbles.size();

  for (auto it = textBubbles.begin() ; it != textBubbles.end() ;)
  {
    TextBubble* textBubble = *it;

    if (textBubble->update(delta))
      ++it;
    else
    {
      it = textBubbles.erase(it);
      delete textBubble;
    }
  }
  if (oldLength != textBubbles.size())
    emit textBubblesChanged();
}

void TextBubblesComponent::addTextBubble(DynamicObject* target, const QString& content, qint64 duration, const QString& color)
{
  TextBubble* textBubble = nullptr;
  bool isNew = true;

  for (auto it = textBubbles.begin() ; it != textBubbles.end() ; ++it)
  {
    if ((*it)->getTarget() == target)
    {
      textBubble = *it;
      isNew = false;
      break ;
    }
  }
  if (!textBubble)
    textBubble = new TextBubble(this, target);
  textBubble->initialize(content, duration, color);
  if (isNew)
  {
    textBubbles << textBubble;
    emit textBubblesChanged();
  }
}

void TextBubblesComponent::unregisterDynamicObject(DynamicObject* object)
{
  auto oldLength = textBubbles.size();

  for (auto it = textBubbles.begin() ; it != textBubbles.end() ;)
  {
    if ((*it)->getTarget() == object)
    {
      delete *it;
      textBubbles.erase(it);
    }
  }
  if (oldLength != textBubbles.size())
    emit textBubblesChanged();
  InteractionComponent::unregisterDynamicObject(object);
}

//
// Text Bubble
//
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
