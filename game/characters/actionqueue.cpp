#include "actionqueue.h"
#include "game.h"

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
      reset();
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

void ActionQueue::pushInteraction(DynamicObject *target, const QString &interactionName)
{
  queue << (new InteractionAction(character, target, interactionName));
}

void ActionQueue::pushItemUse(DynamicObject *target, const QString &itemSlot)
{
  queue << (new ItemAction(character, target, itemSlot));
}

bool ActionQueue::MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();

  return level->moveTo(character, target);
}

bool ActionQueue::MovementAction::isOver()
{
  return character->getPosition() == target && !character->isMoving();
}

bool ActionQueue::InteractionAction::trigger()
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

bool ActionQueue::ItemAction::trigger()
{
  auto* item = character->getInventory()->getEquippedItem(itemSlot);

  if (item)
  {
    item->useOn(target);
    return true;
  }
  return false;
}
