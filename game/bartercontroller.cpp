#include "bartercontroller.h"
#include "inventory.h"
#include "game.h"

BarterController::BarterController(QObject* parent) : QObject(parent)
{
  currentNpcInventory = nullptr;
  playerStash = new Inventory(this);
  npcStash    = new Inventory(this);
}

void BarterController::initialize(ScriptController *script, Character *player, Character *npc)
{
  this->script = script;
  this->player = player;
  this->npc    = npc;
  addInventory(npc->getObjectName(), npc->getInventory());
}

bool BarterController::tryToBarter()
{
  if (script->hasMethod("onBarterStart"))
    return script->call("onBarterStart").toBool();
  return true;
}

void BarterController::closeBarter()
{
  playerStash->transferTo(player->getInventory());
  npcStash->transferTo(getCurrentNpcInventory());
}

void BarterController::concludeBarter()
{
  npcStash->transferTo(player->getInventory());
  playerStash->transferTo(getCurrentNpcInventory());
}

bool BarterController::agreeToBarter()
{
  if (script && script->hasMethod("acceptBarter"))
  {
    QJSValueList params;

    params << Game::get()->getScriptEngine().newQObject(playerStash);
    params << Game::get()->getScriptEngine().newQObject(npcStash);
    return script->call("acceptBarter", params).toBool();
  }
  return playerStash->evaluateValue(npc, player) >= npcStash->evaluateValue(player, npc);
}

void BarterController::addInventory(const QString &title, Inventory* inventory)
{
  qDebug() << "Adding inventory" << inventory << "with title" << title;
  npcInventoryTitles.insert(inventory, title);
  if (!npcInventories.contains(inventory))
    npcInventories.push_back(inventory);
  emit npcInventoriesChanged();
}

void BarterController::removeInventory(Inventory* inventory)
{
  npcInventories.removeOne(inventory);
  npcInventoryTitles.remove(inventory);
  emit npcInventoriesChanged();
}

QString BarterController::inventoryTitle(Inventory* inventory) const
{
  auto it = npcInventoryTitles.find(inventory);

  return it != npcInventoryTitles.end() ? it.value() : QString();
}

Inventory* BarterController::getCurrentNpcInventory() const
{
  return currentNpcInventory ? currentNpcInventory : npc->getInventory();
}

void BarterController::setCurrentNpcInventory(int index)
{
  if (npcInventories.size() > index)
    currentNpcInventory = npcInventories.at(index);
  else
    currentNpcInventory = npc->getInventory();
  emit currentNpcInventoryChanged();
}

void BarterController::itemTransfer(Inventory *from, Inventory *to, InventoryItem *item, int amount)
{
  if (item)
  {
    bool completeTransfer = amount == -1 || item->getQuantity() == amount;

    if (completeTransfer)
    {
      from->removeItem(item);
      to->addItem(item);
    }
    else
    {
      item->remove(amount);
      to->addItemOfType(item->getItemType(), amount);
    }
  }
  else
    qDebug() << "BarterController::itemTransfer received null instead of an item";
}

void BarterController::moveToNpcInventory(InventoryItem* item, int amount)
{
  itemTransfer(npcStash, getCurrentNpcInventory(), item, amount);
}

void BarterController::moveToNpcStash(InventoryItem* item, int amount)
{
  itemTransfer(getCurrentNpcInventory(), npcStash, item, amount);
}

void BarterController::moveToPlayerInventory(InventoryItem* item, int amount)
{
  itemTransfer(playerStash, player->getInventory(), item, amount);
}

void BarterController::moveToPlayerStash(InventoryItem* item, int amount)
{
  itemTransfer(player->getInventory(), playerStash, item, amount);
}
