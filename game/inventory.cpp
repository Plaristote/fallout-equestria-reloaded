#include "inventory.h"
#include "game.h"
#include <QJsonArray>

Inventory::Inventory(QObject *parent) : QObject(parent)
{
  connect(this, &Inventory::unequippedItem, this, &Inventory::equippedItemsChanged);
  connect(this, &Inventory::equippedItemsChanged, this, &Inventory::totalWeightChanged);
  connect(this, &Inventory::itemPicked, this, &Inventory::itemsChanged);
  connect(this, &Inventory::itemsChanged, this, &Inventory::totalWeightChanged);
  connect(this, &Inventory::itemsChanged, this, &Inventory::totalValueChanged);
}

void Inventory::addItem(InventoryItem* item)
{
  for (auto* inventoryItem : qAsConst(items))
  {
    if (inventoryItem->getItemType() == item->getItemType() && inventoryItem->isGroupable(item))
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
  if (itemSlots.values().contains(item))
    unequipItem(item);
  items.removeAll(item);
  disconnect(item, &InventoryItem::weightChanged, this, &Inventory::totalWeightChanged);
  disconnect(item, &InventoryItem::valueChanged, this, &Inventory::totalValueChanged);
  emit totalWeightChanged();
  emit totalValueChanged();
  emit itemsChanged();
}

void Inventory::destroyItem(InventoryItem *item, int quantity)
{
  int amount = item->getQuantity();

  if (quantity >= amount)
  {
    if (isEquippedItem(item))
      unequipItem(item, true);
    else
      removeItem(item);
    item->deleteLater();
  }
  else
    item->remove(quantity);
}

void Inventory::dropItem(InventoryItem *item, int quantity)
{
  int amount = item->getQuantity();
  auto* level = Game::get()->getLevel();
  InventoryItem* droppedItem = item;

  if (amount >= quantity)
  {
    removeItem(item);
    if (level && user)
      emit user->itemDropped(item);
    else
      item->deleteLater();
  }
  else
  {
    if (level)
    {
      droppedItem = new InventoryItem();
      droppedItem->setObjectName(item->getObjectName());
      droppedItem->setItemType(item->getItemType());
      if (quantity > 1)
        droppedItem->add(quantity - 1);
      emit user->itemDropped(item);
    }
    item->remove(quantity);
  }
}

void Inventory::addItemOfType(const QString &name, int quantity)
{
  InventoryItem* item;

  for (auto* inventoryItem : qAsConst(items))
  {
    if (inventoryItem->getItemType() == name && inventoryItem->isGroupable())
    {
      inventoryItem->add(quantity);
      return ;
    }
  }
  item = new InventoryItem(this);
  item->setObjectName(name);
  item->setItemType(name);
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
      if (inventoryItem->getItemType() == name)
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

InventoryItem* Inventory::getItemOfType(const QString &name) const
{
  for (auto* inventoryItem : items)
  {
    if (inventoryItem->getItemType() == name)
      return inventoryItem;
  }
  return nullptr;
}

int Inventory::count(const QString& name) const
{
  int total = 0;

  for (auto* inventoryItem : items)
  {
    if (inventoryItem->getItemType() == name)
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
    total += inventoryItem->getQuantity() * inventoryItem->getValue();
  return total;
}

int Inventory::evaluateValue(Character* buyer, Character* seller) const
{
  int total = 0;

  for (auto* inventoryItem : items)
    total += inventoryItem->getQuantity() * inventoryItem->evaluateValue(buyer, seller);
  return total;

}

QStringList Inventory::getCategoryList() const
{
  QStringList list;

  list << "";
  for (auto* inventoryItem : items)
  {
    if (!list.contains(inventoryItem->getCategory()))
      list << inventoryItem->getCategory();
  }
  return list;
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
      InventoryItem* newItem = new InventoryItem(this);

      newItem->setObjectName(item->getObjectName());
      newItem->setItemType(item->getItemType());
      item->remove(1);
      item = newItem;
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
        oldItem->deleteLater();
      emit unequippedItem(slotName);
    }
  }
}

void Inventory::unequipItem(InventoryItem* item, bool dropped)
{
  for (auto it = itemSlots.begin() ; it != itemSlots.end() ; ++it)
  {
    if (it.value() == item)
    {
      unequipItem(it.key(), dropped);
      break ;
    }
  }
}

InventoryItem* Inventory::getEquippedItem(const QString &slotName) const
{
  if (itemSlots.contains(slotName))
    return itemSlots[slotName];
  return nullptr;
}

bool Inventory::isEquippedItem(InventoryItem* item) const
{
  return std::find(itemSlots.begin(), itemSlots.end(), item) != itemSlots.end();
}

QVector<InventoryItem*> Inventory::getEquippedItems() const
{
  QVector<InventoryItem*> results;
  const auto values = itemSlots.values();

  results.reserve(values.length());
  for (auto* value : values) {
    if (value)
      results << value;
  }
  return results;
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

void Inventory::transferTo(Inventory* other)
{
  const QList<InventoryItem*> list = items;

  for (InventoryItem* item : list)
  {
    removeItem(item);
    other->addItem(item);
  }
  items.clear();
  emit itemsChanged();
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
