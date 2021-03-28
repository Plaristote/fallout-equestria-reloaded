#ifndef  ITEM_USE_ACTION_H
# define ITEM_USE_ACTION_H

# include "base.h"
# include "game/animationSequence/animationsequence.h"

class ItemAction : public ActionBase
{
public:
  ItemAction(Character* character, DynamicObject* target, QString itemSlot) : ActionBase(character), target(target)
  {
    item = character->getInventory()->getEquippedItem(itemSlot);
  }

  ItemAction(Character* character, DynamicObject* target, InventoryItem* item) : ActionBase(character), target(target), item(item)
  {
  }

  int  getApCost() const override;
  bool trigger() override;
  void update() override;

protected:
  void performAction();

  DynamicObject*    target;
  InventoryItem*    item;
  QJSValue          callback;
  AnimationSequence animation;
};

#endif
