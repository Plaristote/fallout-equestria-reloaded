#include "interaction.h"
#include "game.h"
#include "../characterdialog.h"
#include "../lootingcontroller.h"
#include "game/characters/actionqueue.h"
#include "game/mousecursor.h"
#include "game/spell.h"
#include <QDebug>

InteractionComponent::InteractionComponent(QObject *parent) : ParentType(parent)
{
}

void InteractionComponent::registerDynamicObject(DynamicObject* object)
{
  ParentType::registerDynamicObject(object);
}

void InteractionComponent::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
    reinterpret_cast<Character*>(object)->getActionQueue()->reset();
  ParentType::unregisterDynamicObject(object);
}

bool InteractionComponent::openInteractionMenu(DynamicObject* object)
{
  auto entries = object->getAvailableInteractions();

  if (entries.length() > 0)
  {
    //swapMouseMode();
    emit interactionRequired(object, entries);
    return true;
  }
  else
    qDebug() << "no interaction available for object" << object->getObjectName();
  return false;
}

void InteractionComponent::openCountdownDialog(InventoryItem *item)
{
  emit countdownRequired(item);
}

int InteractionComponent::getInteractionDistance(DynamicObject* target, const QString& interactionType)
{
  int distance;

  if (interactionType == "look")
  {
    int perception = getPlayer()->getStatistics()->property("perception").toInt();

    distance = perception;
  }
  else if (interactionType == "push" || interactionType == "talk-to")
    distance = 3;
  else
    distance = target->getInteractionDistance();
  return distance;
}

void InteractionComponent::interactOrderReceived(DynamicObject* target, const QString& interactionType)
{
  int   distance = getInteractionDistance(target, interactionType);
  auto* player = Game::get()->getPlayer();
  auto* actions = player->getActionQueue();

  setDefaultMovementMode();
  actions->reset();
  actions->pushReach(target, static_cast<float>(distance));
  actions->pushInteraction(target, interactionType);
  actions->start();
  if (!actions->isEmpty())
    swapMouseMode();
}

int InteractionComponent::getTargetMode() const
{
  switch (interactionType)
  {
  case ItemUse:
    if (activeItem->usesZoneTarget())
      return ZoneTarget;
    else if (activeItem->getCategory() == "weapon")
      return CharacterTarget;
    break ;
  case SpellUse:
    return Spell::requireSpell(activeSkill).getTargetMode();
  case SkillUse:
  case NoInteraction:
    break ;
  }
  return ParentType::getTargetMode();
}

void InteractionComponent::objectClicked(DynamicObject* object)
{
  switch (mouseMode)
  {
  case InteractionCursor:
    openInteractionMenu(object);
    break ;
  case TargetCursor:
    switch (interactionType)
    {
    case ItemUse:
      useItemOn(object);
      break ;
    case SkillUse:
      useSkillOn(Game::get()->getPlayer(), object, activeSkill);
      break ;
    case SpellUse:
      useSpellOn(Game::get()->getPlayer(), object, activeSkill);
      break ;
    }
    resetInteractionMode();
    break ;
  }
}

void InteractionComponent::initializeDialog(Character* npc)
{
  initializeDialog(npc, npc->getDialogName());
}

void InteractionComponent::initializeDialog(DynamicObject* object, const QString& dialogName)
{
  if (object)
  {
    Character*       player = Game::get()->getPlayer();
    CharacterDialog* dialog = new CharacterDialog(this);

    player->lookAt(object);
    if (object->isCharacter())
    {
      Character* npc = reinterpret_cast<Character*>(object);

      npc->lookAt(player);
      player->getFieldOfView()->setCharacterDetected(npc);
    }
    if (dialog->load(dialogName, player, object))
      emit startDialog(dialog);
    else
      delete dialog;
  }
  else
    qDebug() << "/!\\ Called level.initializeDialog with invalid parameters.";
}


void InteractionComponent::initializeLooting(StorageObject* target)
{
  Character* player = Game::get()->getPlayer();
  LootingController* controller = new LootingController(this);

  if (target->onInspectInventory(player))
  {
    controller->initialize(player, target);
    emit startLooting(controller);
  }
}

void InteractionComponent::targetTileClicked(int x, int y)
{
  switch (interactionType)
  {
  case ItemUse:
    useItemAt(x, y);
    break ;
  case SpellUse:
    useSpellAt(getPlayer(), x, y, activeSkill);
    break ;
  default:
    break ;
  }
}

void InteractionComponent::tileClicked(int x, int y)
{
  switch (mouseMode)
  {
  case MovementCursor:
    movePlayerTo(x, y);
    break ;
  case TargetCursor:
    if (getTargetMode() == ZoneTarget)
      targetTileClicked(x, y);
    break ;
  default:
    break ;
  }
}
