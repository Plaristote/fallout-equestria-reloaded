#include "animationloader.h"

GroupAnimationPart::GroupAnimationPart()
{
}

bool GroupAnimationPart::matches(const QJSValue& descriptor)
{
  return descriptor.isArray();
}

void GroupAnimationPart::initialize(QJSValue& value)
{
  loadAnimationGroup(*this, value);
}

void GroupAnimationPart::addAnimationPart(IAnimationPart* part)
{
  part->setParent(nullptr);
  parts << QSharedPointer<IAnimationPart>(part);
}

void GroupAnimationPart::start()
{
  for (const auto& part : parts)
    part->start();
}

bool GroupAnimationPart::isOver() const
{
  for (const auto& part : parts)
  {
    if (!part->isOver())
      return false;
  }
  return true;
}
