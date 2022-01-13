#ifndef INVENTORYITEM_H
#define INVENTORYITEM_H

#include "../dynamicobject.h"
#include <QJSValue>
#include <QObject>

class InventoryItem : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(QString     icon        READ getIcon NOTIFY itemTypeChanged)
  Q_PROPERTY(QString     category    READ getCategory NOTIFY itemTypeChanged)
  Q_PROPERTY(QString     itemType    READ getItemType WRITE setItemType NOTIFY itemTypeChanged)
  Q_PROPERTY(QString     description READ getDescription NOTIFY itemTypeChanged)
  Q_PROPERTY(int         ammo        MEMBER ammo NOTIFY ammoChanged)
  Q_PROPERTY(int         maxAmmo     MEMBER maxAmmo NOTIFY ammoChanged)
  Q_PROPERTY(int         weight      READ getWeight  NOTIFY weightChanged)
  Q_PROPERTY(int         quantity    MEMBER quantity NOTIFY quantityChanged)
  Q_PROPERTY(int         value       READ getValue NOTIFY valueChanged)
  Q_PROPERTY(bool        isVirtual   READ isVirtual NOTIFY virtualChanged)
  Q_PROPERTY(QStringList useModes    READ getUseModes NOTIFY useModesChanged)
  Q_PROPERTY(QString     useMode     MEMBER useMode NOTIFY useModeChanged)
  Q_PROPERTY(QString     defaultUseMode READ getDefaultMode NOTIFY itemTypeChanged)
  Q_PROPERTY(bool        requiresTarget READ requiresTarget NOTIFY useModeChanged)
  Q_PROPERTY(bool        zoneTarget     READ usesZoneTarget NOTIFY useModeChanged)
  Q_PROPERTY(int         zoneSize       READ getZoneTargetSize NOTIFY useModeChanged)
public:
  explicit InventoryItem(QObject* parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  void setItemType(const QString& value) { itemType = value; emit itemTypeChanged(); }
  const QString& getItemType() const { return itemType; }
  Q_INVOKABLE void add(int quantity = 1);
  Q_INVOKABLE bool remove(int quantity = 1);
  int getInteractionDistance() const override { return 0; }
  QStringList getAvailableInteractions() override;
  int         getWeight() const;
  int         getValue() const;
  int         getQuantity() const { return quantity; }
  QString     getCategory() const;
  QString     getIcon() const;
  QString     getDescription() const;
  QString     getDefaultMode() const;
  bool        isGroupable(InventoryItem* = nullptr);
  bool        isBlockingPath() const override { return false; }
  bool        isVirtual() const { return virtualItem; }
  void        setVirtual(bool value) { virtualItem = value; emit virtualChanged(); }
  QStringList getUseModes() const;
  bool        requiresTarget() const;
  bool        usesZoneTarget() const;
  int         getZoneTargetSize() const;

  Q_INVOKABLE bool canEquipInSlot(const QString&);
  void onEquippedBy(Character*, bool on);

  Q_INVOKABLE int            evaluateValue(Character* buyer, Character* seller);
  Q_INVOKABLE int            getActionPointCost();
  Q_INVOKABLE bool           isCombatItem();
  Q_INVOKABLE bool           isInRange(DynamicObject* target);
  Q_INVOKABLE float          getRange() const;
  Q_INVOKABLE bool           isValidTarget(DynamicObject*);
  QJSValue                   useOn(DynamicObject* target);
  QJSValue                   useAt(int x, int y);
  Q_INVOKABLE void           useFromInventory(QString mode = "use");
  Q_INVOKABLE int            getUseSuccessRate(DynamicObject* target);
  Q_INVOKABLE int            getUseAtSuccessRate(int x, int y);
  Q_INVOKABLE DynamicObject* getOwner() const;
  Q_INVOKABLE void           setCountdown(int value);
  Q_INVOKABLE void           swapUseMode();
  void                       resetUseMode();
  void                       setUseMode(const QString&);
  const QString&             getUseMode() const { return useMode; }

  Q_INVOKABLE void useReload(bool refill = true);
  Q_INVOKABLE void useUnload() { useReload(false); }

signals:
  void itemTypeChanged();
  void nameChanged();
  void weightChanged();
  void quantityChanged();
  void valueChanged();
  void virtualChanged();
  void useModesChanged();
  void useModeChanged();
  void ammoChanged();

private slots:
  void updateScript();
  void updateSprite();

protected:
  bool        defaultLookInteraction() override;
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "items"; }

private:
  QString itemType;
  QString useMode;
  int quantity;
  int ammo = 0;
  int maxAmmo = 0;
  bool virtualItem = false;
};

#endif // INVENTORYITEM_H
