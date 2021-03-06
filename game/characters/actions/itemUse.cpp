#include "itemUse.h"
#include "game/animationSequence/objectanimationpart.h"
#include "game.h"

bool ItemAction::trigger()
{
  InventoryItem* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
  {
    QJSValue result = item->useOn(target);

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
    qDebug() << "no item to uqe";
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
  if (callback.isCallable())
  {
    bool success = Game::get()->scriptCall(callback, QJSValueList(), "ItemAction::performAction").toBool();

    state = success ? Done : Interrupted;
  }
  else
  {
    qDebug() << "ItemAction::performAction: missing callback.";
    state = Interrupted;
  }
}

int ItemAction::getApCost() const
{
  InventoryItem* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
    return item->getActionPointCost();
  return 2;
}
