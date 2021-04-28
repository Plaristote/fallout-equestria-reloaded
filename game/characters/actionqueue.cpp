#include "actionqueue.h"
#include "game.h"
#include "actions/itemUse.h"
#include "actions/skillUse.h"
#include "actions/interaction.h"
#include "actions/movement.h"
#include "actions/reach.h"
#include "actions/reachcase.h"
#include "actions/sliding.h"

ActionQueue::ActionQueue(QObject *parent) : QObject(parent), character(reinterpret_cast<Character*>(parent))
{

}

ActionQueue::~ActionQueue()
{
  for (auto* entry : queue)
    delete entry;
  for (auto* entry : stash)
    delete entry;
}

void ActionQueue::update()
{
  if (!queue.empty())
    runQueue();
  if (!stash.empty())
    clearStash();
}

void ActionQueue::runQueue()
{
  auto* action = queue.first();

  resetFlag = false;
  action->update();
  if (!resetFlag)
  {
    switch (action->getState())
    {
    case ActionBase::Done:
      onActionOver();
      delete action;
      break ;
    case ActionBase::Interrupted:
      reset();
      emit queueCompleted();
      break ;
    case ActionBase::Pending:
    case ActionBase::InProgress:
      break ;
    }
  }
}

void ActionQueue::clearStash()
{
  for (auto* entry : stash)
    delete entry;
  stash.clear();
}

void ActionQueue::pause()
{
  if (queue.size() > 0)
    queue.first()->interrupt();
}

void ActionQueue::unpause()
{
  if (queue.size() > 0)
    queue.first()->trigger();
}

bool ActionQueue::canInterrupt() const
{
  if (!queue.isEmpty())
    return queue.first()->canInterrupt();
  return true;
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
    auto* action = queue.first();

    if (action->getState() != ActionBase::Pending)
      return true;
    if (!(action->trigger()))
    {
      reset();
      emit queueCompleted();
    }
  }
  return queue.size() > 0;
}

void ActionQueue::reset()
{
  if (queue.length() > 0)
    queue.front()->interrupt();
  stash.reserve(queue.size());
  for (auto* entry : qAsConst(queue))
    stash << entry;
  queue.clear();
  resetFlag = true;
}

void ActionQueue::pushMovement(QPoint target)
{
  queue << (new MovementAction(character, target));
}

int ActionQueue::getMovementApCost(QPoint target) const
{
  LevelGrid* grid;
  QList<QPoint> path;
  QPoint from = character->getPosition();

  if (from == target)
    return 0;
  grid = Game::get()->getLevel()->getGrid();
  if (grid->findPath(from, target, path))
    return path.size();
  return -1;
}

void ActionQueue::pushReach(DynamicObject *target, float range)
{
  queue << (new ReachAction(character, target, range));
}

void ActionQueue::pushReach(DynamicObject *target, float range, QJSValue caseCompare)
{
  queue << (new ReachAction(character, target, range, caseCompare));
}

void ActionQueue::pushReachCase(int x, int y, float range)
{
  queue << (new ReachCaseAction(character, QPoint(x, y), range));
}

void ActionQueue::pushReachCase(int x, int y, float range, QJSValue caseCompare)
{
  queue << (new ReachCaseAction(character, QPoint(x, y), range, caseCompare));
}

int ActionQueue::getReachApCost(DynamicObject *target, float range) const
{
  return ReachAction(character, target, range).getApCost();
}

int ActionQueue::getReachApCost(DynamicObject *target, float range, QJSValue caseCompare)
{
  return ReachAction(character, target, range, caseCompare).getApCost();
}

int ActionQueue::getReachCaseApCost(int x, int y, float range) const
{
  return (ReachCaseAction(character, QPoint(x, y), range)).getApCost();
}

int ActionQueue::getReachCaseApCost(int x, int y, float range, QJSValue caseCompare)
{
  return (ReachCaseAction(character, QPoint(x, y), range, caseCompare)).getApCost();
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

void ActionQueue::pushItemUse(DynamicObject *target, InventoryItem* item)
{
  queue << (new ItemAction(character, target, item));
}

void ActionQueue::pushItemUseAt(int x, int y, const QString &itemSlot)
{
  queue << (new ItemZoneAction(character, QPoint(x, y), itemSlot));
}

void ActionQueue::pushItemUseAt(int x, int y, InventoryItem *item)
{
  queue << (new ItemZoneAction(character, QPoint(x, y), item));
}

int ActionQueue::getItemUseApCost(DynamicObject *target, const QString &itemSlot) const
{
  return ItemAction(character, target, itemSlot).getApCost();
}

void ActionQueue::pushSkillUse(DynamicObject *target, const QString &skillName)
{
  queue << (new SkillAction(character, target, skillName));
}

int ActionQueue::getSkillUseApCost(DynamicObject* target, const QString& skillName) const
{
  return SkillAction(character, target, skillName).getApCost();
}

void ActionQueue::pushSliding(QPoint target)
{
  queue << (new SlidingAction(character, target));
}
