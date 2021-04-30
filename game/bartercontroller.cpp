#include "bartercontroller.h"
#include "inventory.h"
#include "game.h"

BarterController::BarterController(QObject* parent) : QObject(parent)
{
  playerStash = new Inventory(this);
  npcStash    = new Inventory(this);
}

void BarterController::initialize(ScriptController *script, Character *player, Character *npc)
{
  this->script = script;
  this->player = player;
  this->npc    = npc;
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
  npcStash->transferTo(npc->getInventory());
}

void BarterController::concludeBarter()
{
  npcStash->transferTo(player->getInventory());
  playerStash->transferTo(npc->getInventory());
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
  return playerStash->getTotalValue() >= npcStash->getTotalValue();
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
    qDebug() << "BarterController::itemTransfer received a null instead of an item";
}

void BarterController::moveToNpcInventory(InventoryItem* item, int amount)
{
  itemTransfer(npcStash, npc->getInventory(), item, amount);
}

void BarterController::moveToNpcStash(InventoryItem* item, int amount)
{
  itemTransfer(npc->getInventory(), npcStash, item, amount);
}

void BarterController::moveToPlayerInventory(InventoryItem* item, int amount)
{
  itemTransfer(playerStash, player->getInventory(), item, amount);
}

void BarterController::moveToPlayerStash(InventoryItem* item, int amount)
{
  itemTransfer(player->getInventory(), playerStash, item, amount);
}
