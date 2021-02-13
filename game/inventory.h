#ifndef  INVENTORY_H
# define INVENTORY_H

# include <QObject>
# include <QQmlListProperty>
# include "inventoryitem.h"

class Inventory : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QQmlListProperty<InventoryItem> items READ getQmlItems NOTIFY itemsChanged)
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

  Q_INVOKABLE int count(const QString& name) const;

  int getTotalWeight() const;
  int getTotalValue() const;

signals:
  void itemsChanged();
  void totalWeightChanged();
  void totalValueChanged();

private:
  QList<InventoryItem*> items;
};

#endif // INVENTORY_H
