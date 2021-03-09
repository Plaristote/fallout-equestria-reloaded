#include "interaction.h"
#include "game.h"
#include "../characterdialog.h"
#include "../lootingcontroller.h"
#include "game/characters/actionqueue.h"

InteractionComponent::InteractionComponent(QObject *parent) : GridComponent(parent)
{

}

void InteractionComponent::registerDynamicObject(DynamicObject* object)
{
  GridComponent::registerDynamicObject(object);
}

void InteractionComponent::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
    reinterpret_cast<Character*>(object)->getActionQueue()->reset();
  GridComponent::unregisterDynamicObject(object);
}

bool InteractionComponent::openInteractionMenu(DynamicObject* object)
{
  auto entries = object->getAvailableInteractions();

  if (entries.length() > 0)
  {
    swapMouseMode();
    emit interactionRequired(object, entries);
    return true;
  }
  else
    qDebug() << "no interaction available for object" << object->getObjectName();
  return false;
}

void InteractionComponent::interactOrderReceived(DynamicObject* target, const QString& interactionType)
{
  auto  position = target->getInteractionPosition();
  auto* player = Game::get()->getPlayer();
  auto* actions = player->getActionQueue();

  actions->reset();
  actions->pushMovement(position);
  actions->pushInteraction(target, interactionType);
  actions->start();
}

void InteractionComponent::swapMouseMode()
{
  switch (mouseMode)
  {
    case InteractionCursor:
    case TargetCursor:
      mouseMode = MovementCursor;
      break ;
    default:
      mouseMode = InteractionCursor;
      break ;
  }
  if (activeItem) {
    activeItem = nullptr;
    emit activeItemChanged();
  }
  activeSkill = "";
  emit mouseModeChanged();
}

void InteractionComponent::setActiveItem(const QString& slotName)
{
  mouseMode = TargetCursor;
  activeItemSlot = slotName;
  activeItem = Game::get()->getPlayer()->getInventory()->getEquippedItem(slotName);
  activeItem->getItemType();
  activeSkill = "";
  emit activeItemChanged();
  emit mouseModeChanged();
}

int InteractionComponent::getTargetMode() const
{
  if (activeItem && activeItem->getItemType() == "weapon")
    return CharacterTarget;
  return AnyTarget;
}

void InteractionComponent::objectClicked(DynamicObject* object)
{
  switch (mouseMode)
  {
  case InteractionCursor:
    openInteractionMenu(object);
    break ;
  case TargetCursor:
    if (activeItem)
      useItemOn(object);
    else if (activeSkill.length() > 0)
      useSkillOn(Game::get()->getPlayer(), object, activeSkill);
    else
      qDebug() << "TODO missing behaviour for target cursor";
    break ;
  }
}

void InteractionComponent::useSkill(const QString &skill)
{
  if (skill == "sneak")
    qDebug() << "TODO: implement sneak skill";
  else
  {
    activeSkill = skill;
    mouseMode = TargetCursor;
    activeItem = nullptr;
    emit activeItemChanged();
    emit mouseModeChanged();
  }
}

void InteractionComponent::useSkillOn(Character* user, DynamicObject* target, const QString &skill)
{
  auto* actions = user->getActionQueue();

  actions->reset();
  actions->pushMovement(target->getInteractionPosition());
  actions->pushSkillUse(target, activeSkill);
  if (actions->start())
    swapMouseMode();
  activeSkill = "";
}

void InteractionComponent::useItemOn(DynamicObject* target)
{
  auto* actions = Game::get()->getPlayer()->getActionQueue();

  actions->reset();
  if (!activeItem->isInRange(target))
  {
    auto  position = target->getInteractionPosition();

    actions->pushMovement(position);
  }
  actions->pushItemUse(target, activeItemSlot);
  if (actions->start())
    swapMouseMode();
}

void InteractionComponent::initializeDialog(Character* npc)
{
  Character*       player = Game::get()->getPlayer();
  CharacterDialog* dialog = new CharacterDialog(this);

  dialog->load(npc->getDialogName(), player, npc);
  emit startDialog(dialog);
}

void InteractionComponent::initializeLooting(StorageObject* target)
{
  Character* player = Game::get()->getPlayer();
  LootingController* controller = new LootingController(this);

  controller->initialize(player, target);
  emit startLooting(controller);
}
