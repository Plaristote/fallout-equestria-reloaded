#include "targetcomponent.h"

TargetComponent::TargetComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &CursorComponent::mouseModeChanged, this, &TargetComponent::updateTargetList);
}

void TargetComponent::registerDynamicObject(DynamicObject* object)
{
  ParentType::registerDynamicObject(object);
  switch (mouseMode)
  {
    case TargetCursor:
    case InteractionCursor:
      updateTargetList();
      break ;
    default:
      break ;
  }
}

void TargetComponent::unregisterDynamicObject(DynamicObject* object)
{
  targetList.unregisterDynamicObject(object);
  ParentType::unregisterDynamicObject(object);
}

void TargetComponent::updateTargetList()
{
  switch (mouseMode)
  {
    default:
      targetList.reset();
      break ;
    case TargetCursor:
      targetList.findNearbyTargets(getPotentialTargets());
      return ;
    case InteractionCursor:
      targetList.findNearbyTargets(allDynamicObjects());
      break ;
  }
}

QVector<DynamicObject*> TargetComponent::getPotentialTargets() const
{
  return findDynamicObjects(
    std::bind(&TargetComponent::isPotentialTarget, this, std::placeholders::_1)
  );
}


bool TargetComponent::isPotentialTarget(const DynamicObject& object) const
{
  if (mouseMode == TargetCursor && getTargetMode() == CharacterTarget)
    return object.isCharacter() && reinterpret_cast<const Character&>(object).isAlive() && ParentType::isPotentialTarget(object);
  return ParentType::isPotentialTarget(object);
}
