#include "storageobject.h"

StorageObject::StorageObject(QObject* parent) : DynamicObject(parent)
{
  inventory = new Inventory(this);
}

void StorageObject::load(const QJsonObject& data)
{
  inventory->load(data["inventory"].toObject());
  DynamicObject::load(data);
}

void StorageObject::save(QJsonObject& data) const
{
  QJsonObject inventoryData;

  inventory->save(inventoryData);
  data["inventory"] = inventoryData;
  DynamicObject::save(data);
}

void StorageObject::updateTasks(qint64 delta)
{
  DynamicObject::updateTasks(delta);
  for (InventoryItem* item : inventory->getItems())
    item->updateTasks(delta);
  for (InventoryItem* item : inventory->getEquippedItems())
    item->updateTasks(delta);
}

QStringList StorageObject::getAvailableInteractions()
{
  QStringList results = DynamicObject::getAvailableInteractions();

  if (metaObject()->className() == QString("StorageObject"))
    results.prepend("use");
  return results;
}
