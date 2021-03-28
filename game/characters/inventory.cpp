#include "inventory.h"

CharacterInventory::CharacterInventory(QObject *parent) : ParentType(parent)
{
  connect(inventory, &Inventory::unequippedItem, this, &CharacterInventory::initializeEmptySlot);
  connect(this, &DynamicObject::scriptNameChanged, this, &CharacterInventory::updateInventorySlots);
  connect(this, &DynamicObject::scriptNameChanged, this, &CharacterInventory::initializeEmptySlots);
}

void CharacterInventory::initializeEmptySlots()
{
  initializeEmptySlot("use-1");
  initializeEmptySlot("use-2");
}

void CharacterInventory::initializeEmptySlot(const QString& slotName)
{
  if (inventory->getEquippedItem(slotName) == nullptr)
  {
    InventoryItem* item = new InventoryItem(this);

    item->setObjectName(getDefaultItemForSlot(slotName));
    item->setItemType(item->getObjectName());
    item->setVirtual(true);
    inventory->equipItem(item, slotName);
  }
}

QString CharacterInventory::getDefaultItemForSlot(const QString& name)
{
  if (script && script->hasMethod("getDefaultItem"))
    return script->call("getDefaultItem", QJSValueList() << name).toString();
  return "melee";
}

void CharacterInventory::updateInventorySlots()
{
  QMap<QString, QString> slotTypes({{"armor", "armor"},{"saddle","saddle"},{"use-1","any"},{"use-2","any"}});

  inventory->setSlots(slotTypes);
}
