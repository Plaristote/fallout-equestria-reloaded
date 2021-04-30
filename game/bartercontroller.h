#ifndef  BARTERCONTROLLER_H
# define BARTERCONTROLLER_H

# include <QObject>

class ScriptController;
class Character;
class Inventory;
class InventoryItem;

class BarterController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character*  player      MEMBER player      NOTIFY initialized)
  Q_PROPERTY(Character*  npc         MEMBER npc         NOTIFY initialized)
  Q_PROPERTY(Inventory*  playerStash MEMBER playerStash NOTIFY initialized)
  Q_PROPERTY(Inventory*  npcStash    MEMBER npcStash    NOTIFY initialized)
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

signals:
  void initialized();

private:
  void itemTransfer(Inventory* from, Inventory* to, InventoryItem* item, int amount);

  ScriptController* script;
  Character*        player;
  Character*        npc;
  Inventory*        playerStash;
  Inventory*        npcStash;
};

#endif // BARTERCONTROLLER_H
