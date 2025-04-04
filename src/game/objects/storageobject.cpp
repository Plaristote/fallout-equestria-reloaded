#include "storageobject.h"
#include "game.h"

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
  {
    results.prepend("look");
    results.prepend("use");
    results.append("use-skill");
    results.removeDuplicates();
  }
  return results;
}

bool StorageObject::onInspectInventory(Character* user)
{
  if (script && script->hasMethod("onInspectInventory"))
    return script->call("onInspectInventory", QJSValueList() << user->asJSValue()).toBool();
  return true;
}

bool StorageObject::onTakeItem(Character* user, InventoryItem* item, int quantity)
{
  if (!user || !item)
    return false;
  if (script && script->hasMethod("onTakeItem"))
    return script->call("onTakeItem", QJSValueList() << user->asJSValue() << item->asJSValue() << quantity).toBool();
  return true;
}

bool StorageObject::onPutItem(Character* user, InventoryItem* item, int quantity)
{
  if (!user || !item)
    return false;
  if (script && script->hasMethod("onPutItem"))
    return script->call("onPutItem", QJSValueList() << user->asJSValue() << item->asJSValue() << quantity).toBool();
  return true;
}
