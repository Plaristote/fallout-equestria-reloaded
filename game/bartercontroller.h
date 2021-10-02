#ifndef  BARTERCONTROLLER_H
# define BARTERCONTROLLER_H

# include <QObject>
# include <QQmlListProperty>

class ScriptController;
class Character;
class Inventory;
class InventoryItem;

class BarterController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character*  player       MEMBER player      NOTIFY initialized)
  Q_PROPERTY(Character*  npc          MEMBER npc         NOTIFY initialized)
  Q_PROPERTY(Inventory*  playerStash  MEMBER playerStash NOTIFY initialized)
  Q_PROPERTY(Inventory*  npcStash     MEMBER npcStash    NOTIFY initialized)
  Q_PROPERTY(Inventory*  npcInventory READ getCurrentNpcInventory NOTIFY currentNpcInventoryChanged)
  Q_PROPERTY(QQmlListProperty<Inventory> npcInventories     READ getNpcInventories     NOTIFY npcInventoriesChanged)
  Q_PROPERTY(QStringList                 npcInventoryTitles READ getNpcInventoryTitles NOTIFY npcInventoriesChanged)
public:
  explicit BarterController(QObject *parent = nullptr);

  void initialize(ScriptController* script, Character* player, Character* npc);

  Q_INVOKABLE bool tryToBarter();
  Q_INVOKABLE void closeBarter();
  Q_INVOKABLE void concludeBarter();
  Q_INVOKABLE bool agreeToBarter();

  Q_INVOKABLE void moveToPlayerInventory(InventoryItem*, int amount = -1);
  Q_INVOKABLE void moveToPlayerStash(InventoryItem*, int amount = -1);
  Q_INVOKABLE void moveToNpcInventory(InventoryItem*, int amount = -1);
  Q_INVOKABLE void moveToNpcStash(InventoryItem*, int amount = -1);

  Q_INVOKABLE void    addInventory(const QString& title, Inventory*);
  Q_INVOKABLE void    removeInventory(Inventory*);
  Q_INVOKABLE QString inventoryTitle(Inventory*) const;
  Inventory*          getCurrentNpcInventory() const;
  Q_INVOKABLE void    setCurrentNpcInventory(int index);

signals:
  void initialized();
  void npcInventoriesChanged();
  void currentNpcInventoryChanged();

private:
  void itemTransfer(Inventory* from, Inventory* to, InventoryItem* item, int amount);
  QQmlListProperty<Inventory> getNpcInventories() { return QQmlListProperty<Inventory>(this, &npcInventories); }
  QStringList getNpcInventoryTitles() const { return npcInventoryTitles.values(); }

  ScriptController*         script;
  Character*                player;
  Character*                npc;
  Inventory*                playerStash;
  Inventory*                npcStash;
  QMap<Inventory*, QString> npcInventoryTitles;
  QList<Inventory*>         npcInventories;
  Inventory*                currentNpcInventory;
};

#endif // BARTERCONTROLLER_H
