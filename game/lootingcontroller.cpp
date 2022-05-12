#include "lootingcontroller.h"
#include "game.h"

LootingController::LootingController(QObject *parent) : QObject(parent)
{
  connect(this, &LootingController::finished, this, &LootingController::deleteLater);
}

void LootingController::initialize(Character* c, StorageObject* t)
{
  character = c;
  connect(character->getInventory(), &Inventory::totalWeightChanged, this, &LootingController::weightsChanged);
  setTarget(t);
  initializeAvailableTargets(t);
}

void LootingController::initializeAvailableTargets(StorageObject* t)
{
  auto* currentLevel = Game::get()->getLevel();
  QVector<DynamicObject*> allTargets{reinterpret_cast<DynamicObject*>(t)};

  if (currentLevel)
    allTargets = currentLevel->getDynamicObjectsAt(t->getPoint());
  for (DynamicObject* target : allTargets)
  {
    if (target->metaObject()->inherits(StorageObject().metaObject()))
      availableTargets.push_back(target);
  }
  availableTargets.removeOne(character);
  emit availableTargetsChanged();
}

void LootingController::setTarget(StorageObject* t)
{
  target = t;
  emit targetChanged();
  emit weightsChanged();
}

void LootingController::nextTarget()
{
  int currentIndex = availableTargets.indexOf(target);

  currentIndex = currentIndex + 1 == availableTargets.size() ? 0 : currentIndex + 1;
  setTarget(dynamic_cast<StorageObject*>(availableTargets.at(currentIndex)));
}

void LootingController::previousTarget()
{
  int currentIndex = availableTargets.indexOf(target);

  currentIndex = (currentIndex - 1 < 0 ? availableTargets.size() : currentIndex) - 1;
  setTarget(dynamic_cast<StorageObject*>(availableTargets.at(currentIndex)));
}

bool LootingController::takeAll()
{
  if (target->getInventory()->getTotalWeight() <= getCapacityLeft())
  {
    const QList<InventoryItem*> list = target->getInventory()->getItems();

    for (InventoryItem* item : list)
    {
      if (!target->onTakeItem(character, item, item->getQuantity()))
      {
        emit finished();
        return false;
      }
    }
    for (InventoryItem* item : list)
    {
      target->getInventory()->removeItem(item);
      character->getInventory()->addItem(item);
    }
    return true;
  }
  return false;
}

bool LootingController::take(InventoryItem* item, int quantity)
{
  if (item->getWeight() / item->getQuantity() * quantity <= getCapacityLeft())
  {
    if (target->onTakeItem(character, item, quantity))
    {
      if (quantity == item->getQuantity())
      {
        target->getInventory()->removeItem(item);
        character->getInventory()->addItem(item);
        return true;
      }
      else if (quantity < item->getQuantity())
      {
        item->remove(quantity);
        character->getInventory()->addItemOfType(item->getObjectName(), quantity);
        return true;
      }
    }
    else
      emit finished();
  }
  return false;
}

bool LootingController::drop(InventoryItem* item, int quantity)
{
  if (target->onPutItem(character, item, quantity))
  {
    if (item->getQuantity() == quantity)
    {
      character->getInventory()->removeItem(item);
      target->getInventory()->addItem(item);
      return true;
    }
    else if (item->getQuantity() > quantity)
    {
      item->remove(quantity);
      target->getInventory()->addItemOfType(item->getObjectName(), quantity);
      return true;
    }
  }
  else
    emit finished();
  return false;
}
