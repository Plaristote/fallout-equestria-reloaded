#include "inventory.h"
#include <QJsonArray>

Inventory::Inventory(QObject *parent) : QObject(parent)
{
  connect(this, &Inventory::unequippedItem, this, &Inventory::equippedItemsChanged);
  connect(this, &Inventory::equippedItemsChanged, this, &Inventory::totalWeightChanged);
  connect(this, &Inventory::itemPicked, this, &Inventory::itemsChanged);
}

void Inventory::addItem(InventoryItem* item)
{
  for (auto* inventoryItem : qAsConst(items))
  {
    if (inventoryItem->getObjectName() == item->getObjectName() && inventoryItem->isGroupable(item))
    {
      inventoryItem->add(item->getQuantity());
      item->deleteLater();
      return ;
    }
  }
  item->setParent(this);
  items << item;
  connect(item, &InventoryItem::weightChanged, this, &Inventory::totalWeightChanged);
  connect(item, &InventoryItem::valueChanged, this, &Inventory::totalValueChanged);
  emit totalWeightChanged();
  emit totalValueChanged();
  emit itemPicked(item);
}

void Inventory::removeItem(InventoryItem *item)
{
  items.removeAll(item);
  disconnect(item, &InventoryItem::weightChanged, this, &Inventory::totalWeightChanged);
  disconnect(item, &InventoryItem::valueChanged, this, &Inventory::totalValueChanged);
  emit totalWeightChanged();
  emit totalValueChanged();
  emit itemsChanged();
}

void Inventory::addItemOfType(const QString &name, int quantity)
{
  InventoryItem* item;

  for (auto* inventoryItem : qAsConst(items))
  {
    if (inventoryItem->getObjectName() == name && inventoryItem->isGroupable())
    {
      inventoryItem->add(quantity);
      return ;
    }
  }
  item = new InventoryItem(this);
  item->setObjectName(name);
  if (quantity > 1)
    item->add(quantity - 1);
  addItem(item);
}

bool Inventory::removeItemOfType(const QString &name, int quantity)
{
  if (count(name) >= quantity)
  {
    for (auto* inventoryItem : items)
    {
      if (inventoryItem->getObjectName() == name)
      {
        int itemQuantity = inventoryItem->getQuantity();

        if (itemQuantity <= quantity)
        {
          removeItem(inventoryItem);
          inventoryItem->deleteLater();
          quantity -= itemQuantity;
          if (quantity == 0)
            break ;
        }
        else
        {
          inventoryItem->remove(quantity);
          break ;
        }
      }
    }
    emit itemsChanged();
    return true;
  }
  return false;
}

int Inventory::count(const QString& name) const
{
  int total = 0;

  for (auto* inventoryItem : items)
  {
    if (inventoryItem->getObjectName() == name)
      total += inventoryItem->getQuantity();
  }
  return total;
}

int Inventory::getTotalWeight() const
{
  int total = 0;

  for (auto* inventoryItem : items)
    total += inventoryItem->getWeight();
  for (auto* inventoryItem : itemSlots)
  {
    if (inventoryItem)
      total += inventoryItem->getWeight();
  }
  return total;
}

int Inventory::getTotalValue() const
{
  int total = 0;

  for (auto* inventoryItem : items)
    total += inventoryItem->getValue();
  return total;
}

bool Inventory::canEquipItem(InventoryItem *item, const QString &slotName) const
{
  return item && slotTypes.contains(slotName) && item->canEquipInSlot(slotTypes[slotName]);
}

bool Inventory::equipItem(InventoryItem *item, const QString& slotName)
{
  if (canEquipItem(item, slotName))
  {
    auto* oldItem = getEquippedItem(slotName);
    bool  shouldDestroyOldObject = oldItem && oldItem->isVirtual();

    if (item->getQuantity() > 1)
    {
      item->remove(1);
      item = new InventoryItem(this);
      item->setObjectName(item->getObjectName());
    }
    else
      removeItem(item);
    unequipItem(slotName, shouldDestroyOldObject);
    itemSlots[slotName] = item;
    item->onEquippedBy(user, true);
    emit equippedItemsChanged();
    return true;
  }
  return false;
}

void Inventory::unequipItem(const QString &slotName, bool dropped)
{
  if (itemSlots.contains(slotName))
  {
    InventoryItem* oldItem = itemSlots[slotName];

    if (oldItem)
    {
      itemSlots[slotName] = nullptr;
      oldItem->onEquippedBy(user, false);
      if (!dropped)
        addItem(oldItem);
      else
        delete oldItem;
      emit unequippedItem(slotName);
    }
  }
}

InventoryItem* Inventory::getEquippedItem(const QString &slotName) const
{
  if (itemSlots.contains(slotName))
    return itemSlots[slotName];
  return nullptr;
}

void Inventory::setSlots(const QMap<QString, QString>& slotTypes)
{
  this->slotTypes = slotTypes;
  slotNames = slotTypes.keys();
  // Discard removed slots
  for (const QString& slotName : itemSlots.keys())
  {
    if (!slotNames.contains(slotName))
    {
      unequipItem(slotName);
      itemSlots.remove(slotName);
    }
  }
  // Insert new slots
  for (const QString& slotName : slotNames)
  {
    if (!itemSlots.contains(slotName))
      itemSlots.insert(slotName, nullptr);
  }
  emit slotsChanged();
}

void Inventory::load(const QJsonObject& data)
{
  QJsonObject slotsData = data["slots"].toObject();

  for (auto itemDataValue : data["items"].toArray())
  {
    QJsonObject itemData = itemDataValue.toObject();
    InventoryItem* item = new InventoryItem(this);

    item->load(itemData);
    addItem(item);
  }
  for (auto it = slotsData.begin() ; it != slotsData.end() ; ++it)
  {
    QString        slotName = it.key();
    QJsonObject    itemData = it.value().toObject();
    InventoryItem* item     = nullptr;

    if (itemData["hasItem"].toBool())
    {
      item = new InventoryItem(this);
      item->load(itemData);
      item->onEquippedBy(user, true);
    }
    slotTypes.insert(slotName, itemData["slotType"].toString());
    itemSlots.insert(slotName, item);
  }
  slotNames = slotTypes.keys();
}

void Inventory::save(QJsonObject& data) const
{
  QJsonArray array;
  QJsonObject slotsData;

  for (auto* inventoryItem : items)
  {
    QJsonObject itemData;

    inventoryItem->save(itemData);
    array << itemData;
  }
  for (auto it = slotTypes.begin() ; it != slotTypes.end() ; ++it)
  {
    QJsonObject    slotData;
    InventoryItem* item = getEquippedItem(it.key());

    slotData.insert("slotType", it.value());
    slotData.insert("hasItem", item && !item->isVirtual());
    if (item && !item->isVirtual())
      item->save(slotData);
    slotsData.insert(it.key(), slotData);
  }
  data["items"] = array;
  data["slots"] = slotsData;
}
