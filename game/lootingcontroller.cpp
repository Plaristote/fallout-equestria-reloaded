#include "lootingcontroller.h"

LootingController::LootingController(QObject *parent) : QObject(parent)
{
  connect(this, &LootingController::finished, this, &LootingController::deleteLater);
}

void LootingController::initialize(Character* c, StorageObject* t)
{
  character = c;
  target    = t;
  connect(character->getInventory(), &Inventory::totalWeightChanged, this, &LootingController::weightsChanged);
  emit targetChanged();
  emit weightsChanged();
}

bool LootingController::take(InventoryItem* item, int quantity)
{
  if (item->getWeight() / item->getQuantity() * quantity <= getCapacityLeft())
  {
    if (quantity == item->getQuantity())
    {
      target->getInventory()->removeItem(item);
      character->getInventory()->addItem(item);
      return true;
    }
    else if (quantity < item->getQuantity())
    {
      item->remove(quantity);
      character->getInventory()->addItemOfType(item->getObjectName(), quantity);
      return true;
    }
  }
  return false;
}

bool LootingController::drop(InventoryItem* item, int quantity)
{
  if (item->getQuantity() == quantity)
  {
    character->getInventory()->removeItem(item);
    target->getInventory()->addItem(item);
    return true;
  }
  else if (item->getQuantity() > quantity)
  {
    item->remove(quantity);
    target->getInventory()->addItemOfType(item->getObjectName());
    return true;
  }
  return false;
}
