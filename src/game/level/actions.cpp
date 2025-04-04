#include "actions.h"
#include "game/spell.h"
#include "game/characters/actionqueue.h"
#include "game.h"
#include "i18n.h"

ActionsComponent::ActionsComponent(QObject *parent) : ParentType(parent)
{

}

void ActionsComponent::unsetActiveItem()
{
  if (activeItem)
  {
    if (interactionType == ItemUse)
      interactionType = NoInteraction;
    disconnect(activeItem, &InventoryItem::beforeDestroy, this, &ActionsComponent::unsetActiveItem);
    disconnect(activeItem, &InventoryItem::parentChanged, this, &ActionsComponent::unsetActiveItem);
    activeItem = nullptr;
    emit activeItemChanged();
  }
}

void ActionsComponent::setActiveItem(const QString& slotName)
{
  Inventory*     inventory     = Game::get()->getPlayer()->getInventory();
  InventoryItem* newActiveItem = inventory->getEquippedItem(slotName);

  if (activeItem != newActiveItem)
  {
    resetInteractionMode();
    interactionType = ItemUse;
    activeItem = newActiveItem;
    if (activeItem)
    {
      connect(activeItem, &InventoryItem::beforeDestroy, this, &ActionsComponent::unsetActiveItem);
      connect(activeItem, &InventoryItem::parentChanged, this, &ActionsComponent::unsetActiveItem);
    }
  }
  activeItemSlot = slotName;
  mouseMode = TargetCursor;
  emit activeItemChanged();
  emit mouseModeChanged();
  onActiveItemChanged();
}

void ActionsComponent::resetInteractionMode()
{
  interactionType = NoInteraction;
  activeSkill     = "";
  unsetActiveItem();
}

void ActionsComponent::onActiveItemChanged()
{
  if (activeItem)
    qDebug() << "InteractionComponent::onActiveItemChanged" << activeItem->getItemType() << ": " << activeItem->requiresTarget();
  if (activeItem && !activeItem->requiresTarget())
    useItemOn(nullptr);
  else if (activeItem)
    targetList.findItemTargets(activeItem, visibleObjects, visibleCharacters);
}

void ActionsComponent::useSpell(const QString &spellName)
{
  auto& spell = Spell::requireSpell(spellName);
  auto* actions = getPlayer()->getActionQueue();

  resetInteractionMode();
  switch (spell.getTargetMode())
  {
  case AnyTarget:
    actions->reset();
    actions->pushSpellUse(spellName);
    if (actions->start())
      swapMouseMode();
    break ;
  case CharacterTarget:
  case ZoneTarget:
    interactionType = SpellUse;
    mouseMode = TargetCursor;
    activeSkill = spellName;
    targetList.findNearbyTargets(allDynamicObjects());
    emit mouseModeChanged();
    break ;
  }
}

void ActionsComponent::useSkill(const QString &skill)
{
  if (skill == "sneak")
    useSneak(getPlayer());
  else
  {
    interactionType = SkillUse;
    activeSkill = skill;
    mouseMode = TargetCursor;
    activeItem = nullptr;
    targetList.findNearbyTargets(allDynamicObjects());
    emit activeItemChanged();
    emit mouseModeChanged();
  }
}

void ActionsComponent::useSpellOn(Character* user, DynamicObject* target, const QString &spellName)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  actions->pushSpellUseOn(target, spellName);
  if (actions->start())
    swapMouseMode();
}

void ActionsComponent::useSpellAt(Character* user, int posX, int posY, const QString &spellName)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  actions->pushSpellUseAt(posX, posY, spellName);
  if (actions->start())
    swapMouseMode();
}

bool ActionsComponent::canSneak(Character* user)
{
  return findCharacters([user](Character& other)
  {
    if (other.isAlive() && !other.isAlly(user))
    {
      float radius   = other.getFieldOfView()->GetRadius();
      float distance = other.getDistance(user);

      return distance <= radius && other.hasLineOfSight(user);
    }
    return false;
  }).size() == 0;
}

bool ActionsComponent::useSneak(Character* user)
{
  if (canSneak(user))
  {
    user->toggleSneaking(true);
    return true;
  }
  else if (user == getPlayer())
    Game::get()->appendToConsole(I18n::get()->t("messages.cannot-sneak"));
  return false;
}

void ActionsComponent::useSkillOn(Character* user, DynamicObject* target, const QString &skill)
{
  int distance = target->getInteractionDistance();
  auto* actions = user->getActionQueue();

  qDebug() << "Useskillon distance =" << distance;
  actions->reset();
  actions->pushReach(target, static_cast<float>(distance));
  actions->pushSkillUse(target, skill);
  if (actions->start())
    swapMouseMode();
}

void ActionsComponent::useItemOn(DynamicObject* target)
{
  if (activeItem)
    useItemOn(getPlayer(), activeItem, target);
  else
    qDebug() << "InteractionComponent::useItemOn: activeItem is null";
}

void ActionsComponent::useItemOn(InventoryItem *item, DynamicObject *target, const QString& useMode)
{
  DynamicObject* owner = item->getOwner();

  if (owner && owner->isCharacter())
    useItemOn(reinterpret_cast<Character*>(owner), item, target, useMode);
}

void ActionsComponent::useItemOn(Character* user, InventoryItem* item, DynamicObject* target, const QString& useMode)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  if (target && (!item->isInRange(target) || !user->hasLineOfSight(target)))
    actions->pushReach(target, item->getRange());
  actions->pushItemUse(target, item, useMode);
  if (actions->start())
    swapMouseMode();
}

void ActionsComponent::useItemAt(int x, int y)
{
  if (activeItem)
    useItemAt(getPlayer(), activeItem, x, y);
  else
    qDebug() << "InteractionComponent::useItemAt: activeItem is null";
}

void ActionsComponent::useItemAt(InventoryItem *item, int x, int y)
{
  DynamicObject* owner = item->getOwner();

  if (owner && owner->isCharacter())
    useItemAt(reinterpret_cast<Character*>(owner), item, x, y);
}

void ActionsComponent::useItemAt(Character *user, InventoryItem *item, int x, int y)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  if (user->getDistance(QPoint(x, y)) > item->getRange())
    actions->pushReachCase(x, y, item->getRange());
  actions->pushItemUseAt(x, y, item);
  if (actions->start())
    swapMouseMode();
}

void ActionsComponent::pickUpItem(Character* character, InventoryItem* item)
{
  Inventory* inventory = character->getInventory();
  StatModel* statistics = character->getStatistics();

  if (inventory->getTotalWeight() + item->getWeight() <= statistics->get_carryWeight())
  {
    detachObject(item);
    inventory->addItem(item);
  }
  else
    Game::get()->appendToConsole(I18n::get()->t("message.cannot-carry-more"));
}
