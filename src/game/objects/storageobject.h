#ifndef STORAGEOBJECT_H
#define STORAGEOBJECT_H

#include "../dynamicobject.h"
#include "../inventory.h"
#include <QObject>

class StorageObject : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(Inventory* inventory MEMBER inventory NOTIFY inventoryChanged)
public:
  explicit StorageObject(QObject* parent = nullptr);

  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;

  virtual void updateTasks(qint64 delta);
  Inventory* getInventory() { return inventory; }
  virtual QStringList getAvailableInteractions();

  Q_INVOKABLE bool onInspectInventory(Character*);
  Q_INVOKABLE bool onTakeItem(Character*, InventoryItem*, int quantity = 1);
  Q_INVOKABLE bool onPutItem(Character*, InventoryItem*, int quantity = 1);

signals:
  void inventoryChanged();
  void itemDropped(InventoryItem*);

protected:
  Inventory* inventory;
};

#endif // STORAGEOBJECT_H
