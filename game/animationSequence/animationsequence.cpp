#include "animationsequence.h"
#include "animationloader.h"

void AnimationSequence::initialize(QJSValue& value)
{
  if (value.isArray())
    loadAnimationGroup(*this, value);
  else
    qDebug() << "AnimationSequence: received an invalid initializer";
}

void AnimationSequence::start()
{
  if (parts.length() > 0)
    parts.first()->start();
}

void AnimationSequence::addAnimationPart(IAnimationPart* part)
{
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
