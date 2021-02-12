#include "interaction.h"
#include "game.h"
#include "../characterdialog.h"

InteractionComponent::InteractionComponent(QObject *parent) : GridComponent(parent)
{

}

void InteractionComponent::registerDynamicObject(DynamicObject* object)
{
  auto* player = Game::get()->getPlayer();

  if (object == player)
    playerReachedDestinationObserver = connect(player, &Character::reachedDestination, this, &InteractionComponent::startPendingInteraction);
  GridComponent::registerDynamicObject(object);
}

void InteractionComponent::unregisterDynamicObject(DynamicObject* object)
{
  auto* player = Game::get()->getPlayer();

  if (object == player)
    disconnect(playerReachedDestinationObserver);
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

  pendingInteraction.first  = target;
  pendingInteraction.second = interactionType;
  if (player->getPosition() != position)
  {
    if (position.x() != -1 && !moveTo(player, position.x(), position.y()))
    {
      pendingInteraction.first = nullptr;
      Game::get()->appendToConsole("You cannot go there.");
    }
  }
  else
    startPendingInteraction();
}

void InteractionComponent::startPendingInteraction()
{
  if (pendingInteraction.first)
  {
    if (pendingInteraction.second == "talk-to")
      initializeDialog(reinterpret_cast<Character*>(pendingInteraction.first));
    else
      qDebug() << "InteractionComponent::startPendingInteraction: unknown interaciton type";
  }
  else
    qDebug() << "InteractionComponent::startPendingInteraction: No more interaction target";
  pendingInteraction.first = nullptr;
}

void InteractionComponent::initializeDialog(Character* npc)
{
  Character*       player = Game::get()->getPlayer();
  CharacterDialog* dialog = new CharacterDialog(this);

  dialog->load(npc->getDialogName(), player, npc);
  emit startDialog(dialog);
}
