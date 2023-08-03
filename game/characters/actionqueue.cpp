#include "actionqueue.h"
#include "game.h"
#include "game/dices.hpp"
#include "actions/itemUse.h"
#include "actions/skillUse.h"
#include "actions/interaction.h"
#include "actions/movement.h"
#include "actions/reach.h"
#include "actions/reachcase.h"
#include "actions/reachdoor.h"
#include "actions/sliding.h"
#include "actions/waitaction.h"
#include "actions/speak.h"
#include "actions/look.h"
#include "actions/script.h"
#include "actions/spellaction.h"
#include "actions/animation.h"
#define ASSERT_NOT_NULL(context, target) \
  if (!target) { qDebug() << context << "called with null target" << target; return ; }

ActionQueue::ActionQueue(QObject *parent) : QObject(parent), character(reinterpret_cast<Character*>(parent))
{

}

ActionQueue::~ActionQueue()
{
  clearStack();
  clearStash();
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

  if (character->isUnconscious() || !character->isAlive())
  {
    clearStack();
    emit queueCompleted();
    return ;
  }
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

void ActionQueue::clearStack()
{
  for (auto* entry : queue)
    delete entry;
  queue.clear();
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
  auto it = queue.begin();

  if (it != queue.end())
  {
    (*it)->interrupt();
    std::for_each(++it, queue.end(), [](ActionBase* action) { action->canceled(); });
  }
  stash.reserve(queue.size());
  for (auto* entry : qAsConst(queue))
    stash << entry;
  queue.clear();
  resetFlag = true;
}

void ActionQueue::pushMovement(Point target)
{
  queue << (new MovementAction(character, target));
}

void ActionQueue::pushMovement(int x, int y)
{
  pushMovement(x, y, static_cast<unsigned char>(character->getCurrentFloor()));
}

int ActionQueue::getMovementApCost(Point target) const
{
  ZoneGrid& grid = Game::get()->getLevel()->getPathfinder();
  QList<Point> path;
  Point from = character->getPoint();

  if (from == target)
    return 0;
  if (grid.findPath(from, target, path, character))
    return path.size();
  return -1;
}

int ActionQueue::getMovementApCost(int x, int y) const
{
  return getMovementApCost(x, y, character->getPoint().z);
}

void ActionQueue::pushReach(DynamicObject *target, float range)
{
  ASSERT_NOT_NULL("ActionQueue::pushReach", target)
  if (!target->isDoorway())
    queue << (new ReachAction(character, target, range));
  else
    queue << (new ReachDoorAction(character, reinterpret_cast<Doorway*>(target), range));
}

void ActionQueue::pushReach(DynamicObject *target, float range, QJSValue caseCompare)
{
  ASSERT_NOT_NULL("ActionQueue::pushReach", target)
  if (!target->isDoorway())
    queue << (new ReachAction(character, target, range, caseCompare));
  else
    queue << (new ReachDoorAction(character, reinterpret_cast<Doorway*>(target), range, caseCompare));
}

void ActionQueue::pushReachCase(int x, int y, float range)
{
  queue << (new ReachCaseAction(character, Point{x, y, static_cast<unsigned char>(character->getCurrentFloor())}, range));
}

void ActionQueue::pushReachCase(int x, int y, float range, QJSValue caseCompare)
{
  queue << (new ReachCaseAction(character, Point{x, y, static_cast<unsigned char>(character->getCurrentFloor())}, range, caseCompare));
}

void ActionQueue::pushReachCase(int x, int y, int z, float range)
{
  queue << (new ReachCaseAction(character, Point{x, y, static_cast<unsigned char>(z)}, range));
}

void ActionQueue::pushReachCase(int x, int y, int z, float range, QJSValue caseCompare)
{
  queue << (new ReachCaseAction(character, Point{x, y, static_cast<unsigned char>(z)}, range, caseCompare));
}

void ActionQueue::pushReachNear(int x, int y, int z, int range)
{
  QVector<Point> choices;
  LevelGrid*     grid = Game::get()->getLevel()->getFloorGrid(static_cast<unsigned char>(z));
  QPoint         position = character->getPosition();

  choices.reserve((range + 1) * (range + 1) - 1);
  for (int xx = x - range ; xx < x + range ; ++xx) {
    for (int yy = y - range ; yy < y + range ; ++yy) {
      if (xx == position.x() && yy == position.y())
        continue ;
      if (!grid->isOccupied(xx, yy))
        choices.push_back(Point{xx, yy, static_cast<unsigned char>(z)});
    }
  }
  while (choices.size() > 0)
  {
    int it = Dices::Throw(static_cast<unsigned int>(choices.size() - 1));
    Point candidate = choices[it];

    if (getMovementApCost(candidate) > 0)
    {
      pushMovement(candidate);
      return ;
    }
    else
      choices.removeAt(static_cast<int>(it));
  }
}

void ActionQueue::pushReachNear(int x, int y, int range)
{
  pushReachNear(x, y, static_cast<int>(character->getCurrentFloor()), range);
}

int ActionQueue::getReachApCost(DynamicObject *target, float range) const
{
  if (!target)
    return 0;
  if (target->isDoorway())
    return ReachAction(character, reinterpret_cast<Doorway*>(target), range).getApCost();
  return ReachAction(character, target, range).getApCost();
}

int ActionQueue::getReachApCost(DynamicObject *target, float range, QJSValue caseCompare)
{
  if (!target)
    return 0;
  if (target->isDoorway())
    return ReachAction(character, reinterpret_cast<Doorway*>(target), range, caseCompare).getApCost();
  return ReachAction(character, target, range, caseCompare).getApCost();
}

int ActionQueue::getReachCaseApCost(int x, int y, float range) const
{
  return (ReachCaseAction(character, Point{x, y, static_cast<unsigned char>(character->getCurrentFloor())}, range)).getApCost();
}

int ActionQueue::getReachCaseApCost(int x, int y, float range, QJSValue caseCompare)
{
  return (ReachCaseAction(character, Point{x, y, static_cast<unsigned char>(character->getCurrentFloor())}, range, caseCompare)).getApCost();
}

void ActionQueue::pushInteraction(DynamicObject *target, const QString &interactionName)
{
  ASSERT_NOT_NULL("ActionQueue::pushInteraction", target)
  queue << (new InteractionAction(character, target, interactionName));
}

int ActionQueue::getInteractionApCost(DynamicObject* target, const QString &interactionName) const
{
  return InteractionAction(character, target, interactionName).getApCost();
}

void ActionQueue::pushItemUse(DynamicObject *target, const QString &itemSlot)
{
  ASSERT_NOT_NULL("ActionQueue::pushItemUse", target)
  queue << (new ItemAction(character, target, itemSlot));
}

void ActionQueue::pushItemUse(DynamicObject *target, InventoryItem* item, const QString& useMode)
{
  ASSERT_NOT_NULL("ActionQueue::pushItemUse", target)
  queue << (new ItemAction(character, target, item, useMode));
}

void ActionQueue::pushItemUseAt(int x, int y, const QString &itemSlot)
{
  queue << (new ItemZoneAction(character, QPoint(x, y), itemSlot));
}

void ActionQueue::pushItemUseAt(int x, int y, InventoryItem *item)
{
  queue << (new ItemZoneAction(character, QPoint(x, y), item));
}

void ActionQueue::pushSpellUse(const QString &spell)
{
  queue << (new SpellAction(character, spell));
}

void ActionQueue::pushSpellUseOn(DynamicObject *target, const QString &spell)
{
  ASSERT_NOT_NULL("ActionQueue::pushSpellUseOn", target)
  queue << (new SpellAction(character, target, spell));
}

void ActionQueue::pushSpellUseAt(QPoint target, const QString &spell)
{
  queue << (new SpellAction(character, target, spell));
}

int ActionQueue::getSpellUseApCost(const QString &spell)
{
  return SpellAction(character, spell).getApCost();
}

int ActionQueue::getItemUseApCost(DynamicObject *target, const QString &itemSlot) const
{
  return ItemAction(character, target, itemSlot).getApCost();
}

void ActionQueue::pushSkillUse(DynamicObject *target, const QString &skillName)
{
  ASSERT_NOT_NULL("ActionQueue::pushSkillUse", target)
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

void ActionQueue::pushWait(unsigned int seconds)
{
  queue << (new WaitAction(character, seconds));
}

void ActionQueue::pushSpeak(const QString& content, unsigned int duration, const QString& color)
{
  queue << (new SpeakAction(character, content, duration, color));
}

void ActionQueue::pushLookAt(const DynamicObject* target)
{
  ASSERT_NOT_NULL("ActionQueue::pushLookAt", target)
  queue << (new LookAction(character, target->getPosition()));
}

void ActionQueue::pushLookAt(int x, int y)
{
  queue << (new LookAction(character, QPoint(x, y)));
}

void ActionQueue::pushScript(QJSValue callback)
{
  queue << (new ScriptAction(character, callback));
}

void ActionQueue::pushAnimation(const QString& animationName, const QString& postAnimationName)
{
  queue << (new AnimationAction(character, animationName, postAnimationName));
}

void ActionQueue::pushAnimation(QJSValue animation)
{
  queue << (new AnimationAction(character, animation));
}
