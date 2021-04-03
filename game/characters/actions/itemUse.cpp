#include "itemUse.h"
#include "game/animationSequence/objectanimationpart.h"
#include "game.h"

bool ItemAction::trigger()
{
  if (item)
  {
    QJSValue result = item->useOn(target);

    if (target)
      character->lookTo(target->getPosition());
    if (result.isObject())
    {
      QJSValue animationSteps = result.property("steps");

      callback = result.property("callback");
      animation.initialize(animationSteps);
      animation.start();
      state = InProgress;
    }
    else if (result.toBool())
    {
      auto* animationPart = new ObjectAnimationPart;

      callback = item->getScriptObject().property("useOn");
      animationPart->initialize(character, "use", "idle");
      animation.addAnimationPart(animationPart);
      animation.start();
      state = InProgress;
    }
    else
      state = Interrupted;
  }
  else
  {
    qDebug() << "no item to use";
    state = Interrupted;
  }
  return state != Interrupted;
}

void ItemAction::update()
{
  bool animationContinues = animation.update();

  if (!animationContinues && state == InProgress)
    performAction();
}

void ItemAction::performAction()
{
  bool success;

  if (callback.isCallable())
    success = Game::get()->scriptCall(callback, QJSValueList(), "ItemAction::performAction").toBool();
  else
  {
    QJSValueList args;

    if (target)
      args << target->asJSValue();
    callback = item->getScriptObject().property("useOn");
    success = Game::get()->scriptCall(callback, args, "ItemAction::performAction").toBool();
  }
  state = success ? Done : Interrupted;
}

int ItemAction::getApCost() const
{
  if (item)
    return item->getActionPointCost();
  return 2;
}
