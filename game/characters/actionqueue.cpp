#include "actionqueue.h"
#include "game.h"

class MovementAction : public ActionBase
{
public:
  MovementAction(Character* character, QPoint target) : ActionBase(character), target(target)
  {
  }

  int  getApCost() const override;
  bool trigger() override;
  bool isOver() override;

protected:
  QPoint target;
};

class InteractionAction : public ActionBase
{
public:
  InteractionAction(Character* character, DynamicObject* target, QString actionName) : ActionBase(character), target(target), actionName(actionName)
  {
  }

  int  getApCost() const override;
  bool trigger() override;

protected:
  DynamicObject* target;
  QString        actionName;
};

class ItemAction : public ActionBase
{
public:
  ItemAction(Character* character, DynamicObject* target, QString itemSlot) : ActionBase(character), target(target), itemSlot(itemSlot)
  {
  }

  int  getApCost() const override;
  bool trigger() override;

protected:
  DynamicObject* target;
  QString        itemSlot;
};

ActionQueue::ActionQueue(QObject *parent) : QObject(parent), character(reinterpret_cast<Character*>(parent))
{

}

ActionQueue::~ActionQueue()
{
  for (auto* entry : queue)
    delete entry;
}

void ActionQueue::update()
{
  if (!queue.empty())
  {
    auto* action = queue.first();

    if (action->isOver())
    {
      onActionOver();
      delete action;
    }
  }
}

void ActionQueue::onActionOver()
{
  queue.pop_front();
  if (!queue.empty())
  {
    auto* action = queue.first();

    if (action->trigger())
      return ;
    reset();
  }
  emit queueCompleted();
}

bool ActionQueue::start()
{
  if (queue.size() >= 1)
  {
    if (!(queue.first()->trigger()))
    {
      reset();
      emit queueCompleted();
    }
  }
  return !queue.empty();
}

void ActionQueue::reset()
{
  for (auto* entry : queue)
    delete entry;
  queue.clear();
}

void ActionQueue::pushMovement(QPoint target)
{
  queue << (new MovementAction(character, target));
}

int ActionQueue::getMovementApCost(QPoint target) const
{
  LevelGrid* grid;
  QList<QPoint> path;

  if (character->getPosition() == target)
    return 0;
  grid = Game::get()->getLevel()->getGrid();
  grid->findPath(character->getPosition(), target, path);
  return path.size() >  0 ? path.size() : -1;
}

void ActionQueue::pushInteraction(DynamicObject *target, const QString &interactionName)
{
  queue << (new InteractionAction(character, target, interactionName));
}

int ActionQueue::getInteractionApCost(DynamicObject* target, const QString &interactionName) const
{
  return InteractionAction(character, target, interactionName).getApCost();
}

void ActionQueue::pushItemUse(DynamicObject *target, const QString &itemSlot)
{
  queue << (new ItemAction(character, target, itemSlot));
}

int ActionQueue::getItemUseApCost(DynamicObject *target, const QString &itemSlot) const
{
  return ItemAction(character, target, itemSlot).getApCost();
}

bool MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();

  return level->moveTo(character, target);
}

bool MovementAction::isOver()
{
  return character->getPosition() == target && !character->isMoving();
}

int MovementAction::getApCost() const
{
  return character->getCurrentPath().size();
}

bool InteractionAction::trigger()
{
  auto* level = Game::get()->getLevel();
  bool handledByScript = target->triggerInteraction(character, actionName);

  if (!handledByScript && character == level->getPlayer())
  {
    QString typeName = target->metaObject()->className();

    if (actionName == "talk-to" && typeName == "Character")
      level->initializeDialog(reinterpret_cast<Character*>(target));
    else if (actionName == "use" && typeName == "StorageObject")
      level->initializeLooting(reinterpret_cast<StorageObject*>(target));
    else if (actionName == "use" && typeName == "Character" && !(reinterpret_cast<Character*>(target)->isAlive()))
      level->initializeLooting(reinterpret_cast<StorageObject*>(target));
    return false; // Always clear the queue on player interaction
  }
  return handledByScript;
}

int InteractionAction::getApCost() const
{
  return 2;
}

bool ItemAction::trigger()
{
  InventoryItem* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
  {
    item->useOn(target);
    return true;
  }
  else
    qDebug() << "no item to uqe";
  return false;
}

int ItemAction::getApCost() const
{
  InventoryItem* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
    return item->getActionPointCost();
  return 2;
}
