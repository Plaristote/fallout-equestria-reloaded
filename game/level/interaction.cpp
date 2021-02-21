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
  activeItem = nullptr;
  emit mouseModeChanged();
}

void InteractionComponent::setActiveItem(const QString& slotName)
{
  mouseMode = TargetCursor;
  activeItemSlot = slotName;

  activeItem = Game::get()->getPlayer()->getInventory()->getEquippedItem(slotName);
  emit activeItemChanged();
  emit mouseModeChanged();
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
    else
      qDebug() << "TODO missing behaviour for target cursor";
    break ;
  }
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
