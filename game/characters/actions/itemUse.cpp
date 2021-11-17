#include "itemUse.h"
#include "game/animationSequence/objectanimationpart.h"
#include "game.h"
#include "i18n.h"
#include <QDebug>

QJSValue ItemAction::callItemUseOn()
{
  return item->useOn(target);
}

QJSValue ItemZoneAction::callItemUseOn()
{
  return item->useAt(target.x(), target.y());
}

void ItemZoneAction::lookAtTarget()
{
  character->lookTo(getTargetPosition());
}

void ItemAction::lookAtTarget()
{
  if (target)
    character->lookTo(getTargetPosition());
}

bool ItemAction::trigger()
{
  if (item)
  {
    QString backupMode;
    QJSValue result;

    if (useMode != item->getUseMode() && !useMode.isEmpty())
    {
      backupMode = item->getUseMode();
      item->setUseMode(useMode);
    }
    result = callItemUseOn();
    lookAtTarget();
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
    {
      Game::get()->appendToConsole(I18n::get()->t("messages.invalid-target"));
      state = Interrupted;
    }
    if (!backupMode.isEmpty())
      item->setUseMode(backupMode);
  }
  else
  {
    qDebug() << "no item to use";
    state = Interrupted;
  }
  return state != Interrupted;
}

bool ItemAction::canInterrupt() const
{
  return !(state == InProgress && animation.isRunning());
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
