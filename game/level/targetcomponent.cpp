#include "targetcomponent.h"

TargetComponent::TargetComponent(QObject *parent) : ParentType(parent)
{

}

void TargetComponent::registerDynamicObject(DynamicObject* object)
{
  ParentType::registerDynamicObject(object);
}

void TargetComponent::unregisterDynamicObject(DynamicObject* object)
{
  targetList.unregisterDynamicObject(object);
  ParentType::unregisterDynamicObject(object);
}

void TargetComponent::swapMouseMode()
{
  switch (mouseMode)
  {
    case WaitCursor:
      return ;
    case InteractionCursor:
    case TargetCursor:
      targetList.reset();
      break ;
    default:
      targetList.findNearbyTargets(allDynamicObjects());
      break ;
  }
  ParentType::swapMouseMode();
}
