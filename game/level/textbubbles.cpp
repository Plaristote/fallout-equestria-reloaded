#include "textbubbles.h"

TextBubblesComponent::TextBubblesComponent(QObject *parent) : ParentType(parent)
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
  ParentType::update(delta);
}

TextBubble* TextBubblesComponent::getTextBubbleFor(DynamicObject* target) const
{
  for (auto it = textBubbles.begin() ; it != textBubbles.end() ; ++it)
  {
    if ((*it)->getTarget() == target)
      return *it;
  }
  return nullptr;
}

void TextBubblesComponent::addTextBubble(DynamicObject* target, const QString& content, qint64 duration, const QString& color)
{
  TextBubble* textBubble = getTextBubbleFor(target);
  bool isNew = !textBubble;

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
    else
      ++it;
  }
  if (oldLength != textBubbles.size())
    emit textBubblesChanged();
  ParentType::unregisterDynamicObject(object);
}
