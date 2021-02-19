#ifndef  INVENTORY_H
# define INVENTORY_H

# include <QObject>
# include <QQmlListProperty>
# include "inventoryitem.h"

class Character;

class Inventory : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QQmlListProperty<InventoryItem> items READ getQmlItems NOTIFY itemsChanged)
  Q_PROPERTY(QStringList slotNames MEMBER slotNames NOTIFY slotsChanged)
  Q_PROPERTY(int totalWeight READ getTotalWeight NOTIFY totalWeightChanged)
  Q_PROPERTY(int totalValue  READ getTotalValue  NOTIFY totalValueChanged)
public:
  explicit Inventory(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  QQmlListProperty<InventoryItem> getQmlItems() { return QQmlListProperty<InventoryItem>(this, &items); }
  const QList<InventoryItem*>& getItems() const { return items; }

  Q_INVOKABLE void addItem(InventoryItem* item);
  Q_INVOKABLE void removeItem(InventoryItem* item);

  Q_INVOKABLE void addItemOfType(const QString& name, int quantity = 1);
  Q_INVOKABLE bool removeItemOfType(const QString& name, int quantity = 1);

  Q_INVOKABLE bool equipItem(InventoryItem* item, const QString& slotName);
  Q_INVOKABLE bool canEquipItem(InventoryItem* item, const QString& slotName) const;
  Q_INVOKABLE void unequipItem(const QString& slotName, bool dropped = false);
  Q_INVOKABLE InventoryItem* getEquippedItem(const QString& slotName) const;

  Q_INVOKABLE int count(const QString& name) const;

  int getTotalWeight() const;
  int getTotalValue() const;

  void setSlots(const QMap<QString, QString>&);

signals:
  void itemsChanged();
  void totalWeightChanged();
  void totalValueChanged();
  void slotsChanged();
  void equippedItemsChanged();
  void unequippedItem(const QString&);

private:
  QList<InventoryItem*>         items;
  QMap<QString, InventoryItem*> itemSlots;
  QStringList                   slotNames;
  QMap<QString, QString>        slotTypes;
};

#endif // INVENTORY_H
