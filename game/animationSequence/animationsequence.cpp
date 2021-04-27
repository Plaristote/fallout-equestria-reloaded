#include <QJSValue>
#include <QDebug>
#include "animationsequence.h"
#include "objectanimationpart.h"
#include "spriteanimationpart.h"

void AnimationSequence::initialize(QJSValue& value)
{
  unsigned int length = value.property("length").toUInt();

  for (unsigned int i = 0 ; i < length ; ++i)
  {
    QJSValue animationDescriptor = value.property(i);
    QString  type = animationDescriptor.property("type").toString();

    if (type == "Sprite")
    {
      auto* part = new SpriteAnimationPart;

      part->initialize(animationDescriptor);
      addAnimationPart(part);
    }
    else if (type == "Animation")
    {
      auto* part = new ObjectAnimationPart;

      part->initialize(animationDescriptor);
      addAnimationPart(part);
    }
    else
      qDebug() << "ActionAnimation: unknown animation type" << type;
  }
}

void AnimationSequence::start()
{
  if (parts.length() > 0)
    parts.first()->start();
}

void AnimationSequence::addAnimationPart(IAnimationPart* part)
{
  part->setParent(nullptr);
  parts << QSharedPointer<IAnimationPart>(part);
}

bool AnimationSequence::update()
{
  if (parts.length() > 0)
  {
    auto& part = parts.first();

    if (part->isOver())
      return nextAnimation();
    return true;
  }
  return false;
}

bool AnimationSequence::isRunning() const
{
  return !parts.isEmpty();
}

bool AnimationSequence::nextAnimation()
{
  parts.pop_front();
  if (parts.length() > 0)
  {
    parts.first()->start();
    return true;
  }
  return false;
}
