#include "spellaction.h"
#include "game.h"

void SpellAction::lookAtTarget()
{
  if (targetType != NoTarget)
    character->lookTo(getTargetPosition());
}

int SpellAction::getApCost() const
{
  return spell.getActionPointCost(character);
}

bool SpellAction::trigger()
{
  QJSValue result;

  switch (targetType)
  {
  case ObjectTarget:
    result = spell.useOn(character, target);
    break ;
  case PositionTarget:
    result = spell.useAt(character, targetPosition.x(), targetPosition.y());
    break ;
  case NoTarget:
    result = spell.use(character);
    break ;
  }
  return triggerAnimation(result);
}

void SpellAction::performAction()
{
  bool success = false;
  QJSValueList params;

  if (!callback.isCallable())
    callback = spell.triggerUse;
  params << character->asJSValue();
  switch (targetType)
  {
  case ObjectTarget:
    params << target->asJSValue();
    break ;
  case PositionTarget:
    params << targetPosition.x() << targetPosition.y();
    break ;
  case NoTarget:
    break ;
  }
  success = Game::get()->scriptCall(callback, params, "SpellAction::performAction").toBool();
  state = success ? Done : Interrupted;
}

QJSValue SpellAction::getDefaultCallback()
{
  return QJSValue();
}
