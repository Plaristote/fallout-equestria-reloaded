#include "inventory.h"
#include <QJsonArray>

Inventory::Inventory(QObject *parent) : QObject(parent)
{

}

void Inventory::addItem(InventoryItem* item)
{
  for (auto* inventoryItem : items)
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
  emit itemsChanged();
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

  for (auto* inventoryItem : items)
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
  return total;
}

int Inventory::getTotalValue() const
{
  int total = 0;

  for (auto* inventoryItem : items)
    total += inventoryItem->getValue();
  return total;
}

void Inventory::load(const QJsonObject& data)
{
  for (auto itemDataValue : data["items"].toArray())
  {
    QJsonObject itemData = itemDataValue.toObject();
    InventoryItem* item = new InventoryItem(this);

    item->load(itemData);
    addItem(item);
  }
}

void Inventory::save(QJsonObject& data) const
{
  QJsonArray array;

  for (auto* inventoryItem : items)
  {
    QJsonObject itemData;

    inventoryItem->save(itemData);
    array << itemData;
  }
  data["items"] = array;
}

