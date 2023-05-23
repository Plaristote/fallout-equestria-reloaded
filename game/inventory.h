#ifndef  INVENTORY_H
# define INVENTORY_H

# include <QObject>
# include <QQmlListProperty>
# include "objects/inventoryitem.h"

class Character;

class Inventory : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QQmlListProperty<InventoryItem> items READ getQmlItems NOTIFY itemsChanged)
  Q_PROPERTY(QStringList slotNames MEMBER slotNames NOTIFY slotsChanged)
  Q_PROPERTY(QStringList categoryList READ getCategoryList NOTIFY itemsChanged)
  Q_PROPERTY(int totalWeight READ getTotalWeight NOTIFY totalWeightChanged)
  Q_PROPERTY(int totalValue  READ getTotalValue  NOTIFY totalValueChanged)
public:
  explicit Inventory(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;
  void setUser(Character* value) { user = value; }
  Character* getUser() const { return user; }

  QQmlListProperty<InventoryItem> getQmlItems() { return QML_QLIST_CONSTRUCTOR(InventoryItem, items); }
  const QList<InventoryItem*>& getItems() const { return items; }
  QStringList getCategoryList() const;

  Q_INVOKABLE void addItem(InventoryItem* item);
  Q_INVOKABLE void removeItem(InventoryItem* item);
  Q_INVOKABLE void destroyItem(InventoryItem* item, int quantity = 1);
  Q_INVOKABLE void dropItem(InventoryItem* item, int quantity = 1);

  Q_INVOKABLE InventoryItem* addItemOfType(const QString& name, int quantity = 1);
  Q_INVOKABLE bool removeItemOfType(const QString& name, int quantity = 1);
  Q_INVOKABLE InventoryItem* getItemOfType(const QString& name) const;

  Q_INVOKABLE bool equipItem(InventoryItem* item, const QString& slotName);
  Q_INVOKABLE bool canEquipItem(InventoryItem* item, const QString& slotName) const;
  Q_INVOKABLE void unequipItem(const QString& slotName, bool dropped = false);
  Q_INVOKABLE void unequipItem(InventoryItem*, bool dropped = false);
  Q_INVOKABLE InventoryItem* getEquippedItem(const QString& slotName) const;
  QVector<InventoryItem*> getEquippedItems() const;
  Q_INVOKABLE bool isEquippedItem(InventoryItem*) const;
  Q_INVOKABLE QString getEquippedItemSlot(InventoryItem*) const;

  Q_INVOKABLE int count(const QString& name) const;
  Q_INVOKABLE bool contains(InventoryItem* item) const { return items.contains(item); }
  Q_INVOKABLE InventoryItem* findOne(QJSValue filter) const;
  Q_INVOKABLE QJSValue find(QJSValue filter) const;

  int getTotalWeight() const;
  int getTotalValue() const;
  Q_INVOKABLE int evaluateValue(Character* buyer, Character* seller) const;

  void                   setSlots(const QMap<QString, QString>&);
  QMap<QString, QString> getSlots() const { return slotTypes; }
  void transferTo(Inventory*);

signals:
  void itemsChanged();
  void itemPicked(InventoryItem*);
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
  Character*                    user = nullptr;
};

#endif // INVENTORY_H
