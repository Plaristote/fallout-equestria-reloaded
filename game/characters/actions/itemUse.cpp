#include "itemUse.h"

bool ItemAction::trigger()
{
  InventoryItem* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
  {
    item->useOn(target);
    return true;
  }
  else
    qDebug() << "no item to uqe";
  return false;
}

int ItemAction::getApCost() const
{
  InventoryItem* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
    return item->getActionPointCost();
  return 2;
}
