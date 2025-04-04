#ifndef  ITEM_USE_ACTION_H
# define ITEM_USE_ACTION_H

# include "animatedaction.h"

class ItemAction : public AnimatedAction
{
public:
  ItemAction(Character* character, DynamicObject* target, QString itemSlot) : AnimatedAction(character), target(target)
  {
    item = character->getInventory()->getEquippedItem(itemSlot);
  }

  ItemAction(Character* character, DynamicObject* target, InventoryItem* item, QString useMode) : AnimatedAction(character), target(target), item(item), useMode(useMode)
  {
  }

  int  getApCost() const override;
  bool trigger() override;

protected:
  void performAction() override;
  virtual QPoint getTargetPosition() const { return target->getPosition(); }
  virtual void lookAtTarget() override;
  QJSValue getDefaultCallback() override;
  virtual QJSValue callItemUseOn();

  DynamicObject*    target;
  InventoryItem*    item;
  QString           useMode;
};

class ItemZoneAction : public ItemAction
{
public:
  ItemZoneAction(Character* character, QPoint target, QString itemSlot) : ItemAction(character, nullptr, itemSlot), target(target)
  {
  }

  ItemZoneAction(Character* character, QPoint target, InventoryItem* item) : ItemAction(character, nullptr, item, QString()), target(target)
  {
  }

private:
  QPoint getTargetPosition() const override { return target; }
  void lookAtTarget() override;
  QJSValue callItemUseOn() override;

  QPoint target;
};

#endif
