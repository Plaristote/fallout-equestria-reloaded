#ifndef LOOTINGCONTROLLER_H
#define LOOTINGCONTROLLER_H

#include <QObject>
#include "objects/storageobject.h"
#include "character.h"

class LootingController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(StorageObject* target MEMBER target NOTIFY targetChanged)
  Q_PROPERTY(Inventory* inventory READ getInventory NOTIFY targetChanged)
  Q_PROPERTY(Character* character MEMBER character NOTIFY targetChanged)
  Q_PROPERTY(int capacity READ getCharacterCapacity NOTIFY weightsChanged)
  Q_PROPERTY(int usedCapacity READ getCharacterUsedCapacity NOTIFY weightsChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> availableTargets READ getAvailableTargets NOTIFY availableTargetsChanged)
public:
  explicit LootingController(QObject *parent = nullptr);

  void initialize(Character* c, StorageObject* t);

  Inventory* getInventory() { return target->getInventory(); }

  Q_INVOKABLE bool takeAll();
  Q_INVOKABLE bool take(InventoryItem* item, int quantity = 1);
  Q_INVOKABLE bool drop(InventoryItem* item, int quantity = 1);

  int getCharacterCapacity() { return character->getStatistics()->get_carryWeight(); }
  int getCharacterUsedCapacity() { return character->getInventory()->getTotalWeight(); }
  int getCapacityLeft() { return getCharacterCapacity() - getCharacterUsedCapacity(); }

  Q_INVOKABLE void nextTarget();
  Q_INVOKABLE void previousTarget();
  QQmlListProperty<DynamicObject> getAvailableTargets() { return QQmlListProperty<DynamicObject>(this, &availableTargets); }

signals:
  void targetChanged();
  void availableTargetsChanged();
  void weightsChanged();
  void finished();

private:
  void initializeAvailableTargets(StorageObject*);
  void setTarget(StorageObject*);

  StorageObject* target;
  Character*     character;
  QList<DynamicObject*> availableTargets;
};

#endif // LOOTINGCONTROLLER_H
