#include "inventory.h"
#include "cmap/race.h"

CharacterInventory::CharacterInventory(QObject *parent) : ParentType(parent)
{
  connect(inventory, &Inventory::unequippedItem, this, &CharacterInventory::initializeEmptySlot);
  connect(this, &CharacterStatistics::raceChanged, this, &CharacterInventory::updateInventorySlots);
  connect(this, &CharacterStatistics::raceChanged, this, &CharacterInventory::initializeEmptySlots);
  connect(this, &DynamicObject::scriptNameChanged, this, &CharacterInventory::updateInventorySlots);
  connect(this, &DynamicObject::scriptNameChanged, this, &CharacterInventory::initializeEmptySlots);
}

void CharacterInventory::initializeEmptySlots()
{
  const auto itemSlots = inventory->getSlots();

  for (auto it = itemSlots.begin() ; it != itemSlots.end() ; ++it)
  {
    if (it.value() == "any")
      initializeEmptySlot(it.key());
  }
}

void CharacterInventory::initializeEmptySlot(const QString& slotName)
{
  InventoryItem* currentItem = inventory->getEquippedItem(slotName);

  if (currentItem == nullptr || currentItem->isVirtual())
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
  QJSValue scriptResult = scriptCall("getDefaultItem", QJSValueList() << name);

  if (scriptResult.isString())
    return scriptResult.toString();
  if (statistics && statistics->getRaceController())
    return statistics->getRaceController()->getDefaultItemForSlot(statistics, name);
  return "melee";
}

void CharacterInventory::updateInventorySlots()
{
  QMap<QString, QString> slotTypes;

  if (statistics && statistics->getRaceController())
    slotTypes = statistics->getRaceController()->getItemSlots();
  else
    slotTypes = Race().getItemSlots();
  inventory->setSlots(slotTypes);
}

void CharacterInventory::unequipUseSlots()
{
  const auto itemSlots = inventory->getSlots();

  for (auto it = itemSlots.begin() ; it != itemSlots.end() ; ++it)
  {
    InventoryItem* item = inventory->getEquippedItem(it.key());

    if (item && !item->isVirtual())
      inventory->unequipItem(it.key());
  }
}
