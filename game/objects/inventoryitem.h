#ifndef INVENTORYITEM_H
#define INVENTORYITEM_H

#include "../dynamicobject.h"
#include <QJSValue>
#include <QObject>

class InventoryItem : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(QString     category  READ getCategory NOTIFY itemTypeChanged)
  Q_PROPERTY(QString     itemType  READ getItemType WRITE setItemType NOTIFY itemTypeChanged)
  Q_PROPERTY(int         weight    READ getWeight  NOTIFY weightChanged)
  Q_PROPERTY(int         quantity  MEMBER quantity NOTIFY quantityChanged)
  Q_PROPERTY(int         value     READ getValue NOTIFY valueChanged)
  Q_PROPERTY(bool        isVirtual READ isVirtual NOTIFY virtualChanged)
  Q_PROPERTY(QStringList useModes  READ getUseModes NOTIFY useModesChanged)
  Q_PROPERTY(QString     useMode   MEMBER useMode NOTIFY useModeChanged)
  Q_PROPERTY(bool        requiresTarget READ requiresTarget NOTIFY useModeChanged)
public:
  explicit InventoryItem(QObject* parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  void setItemType(const QString& value) { itemType = value; emit itemTypeChanged(); }
  const QString& getItemType() const { return itemType; }
  Q_INVOKABLE void add(int quantity = 1);
  Q_INVOKABLE bool remove(int quantity = 1);
  Q_INVOKABLE QPoint getInteractionPosition() const { return getPosition(); }
  QStringList getAvailableInteractions();
  int         getWeight() const;
  int         getValue() const;
  int         getQuantity() const { return quantity; }
  QString     getCategory() const;
  bool        isGroupable(InventoryItem* = nullptr);
  bool        isVirtual() const { return virtualItem; }
  void        setVirtual(bool value) { virtualItem = value; emit virtualChanged(); }
  QStringList getUseModes() const;
  bool        requiresTarget() const;

  Q_INVOKABLE bool canEquipInSlot(const QString&);
  void onEquippedBy(Character*, bool on);

  Q_INVOKABLE int      getActionPointCost();
  Q_INVOKABLE bool     isCombatItem();
  Q_INVOKABLE bool     isInRange(DynamicObject* target);
  Q_INVOKABLE bool     isValidTarget(DynamicObject*);
  Q_INVOKABLE QJSValue useOn(DynamicObject* target);
  Q_INVOKABLE int      getUseSuccessRate(DynamicObject* target);
  Q_INVOKABLE DynamicObject* getOwner() const;
  Q_INVOKABLE void           setCountdown(int value);

signals:
  void itemTypeChanged();
  void nameChanged();
  void weightChanged();
  void quantityChanged();
  void valueChanged();
  void virtualChanged();
  void useModesChanged();
  void useModeChanged();

private slots:
  void updateScript();
  void updateSprite();

protected:
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "items"; }

private:
  QString itemType;
  QString useMode;
  int quantity;
  bool virtualItem = false;
};

#endif // INVENTORYITEM_H
