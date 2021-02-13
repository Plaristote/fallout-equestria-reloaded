#ifndef STORAGEOBJECT_H
#define STORAGEOBJECT_H

#include "dynamicobject.h"
#include "inventory.h"
#include <QObject>

class StorageObject : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(Inventory* inventory MEMBER inventory NOTIFY inventoryChanged)
public:
  explicit StorageObject(QObject* parent = nullptr);

  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;

  Inventory* getInventory() { return inventory; }
  virtual QStringList getAvailableInteractions();

signals:
  void inventoryChanged();

protected:
  Inventory* inventory;
};

#endif // STORAGEOBJECT_H
