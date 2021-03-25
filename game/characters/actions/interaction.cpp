#include "interaction.h"
#include "game.h"
#include "game/animationSequence/objectanimationpart.h"

bool InteractionAction::trigger()
{
  character->lookTo(target->getPosition());
  if (actionName == "talk-to" || actionName == "look")
    performAction();
  else
  {
    auto* animationPart = new ObjectAnimationPart;

    animationPart->initialize(character, "use", "idle");
    animation.addAnimationPart(animationPart);
    animation.start();
    state = InProgress;
  }
  return state != Interrupted;
}

void InteractionAction::update()
{
  bool animationContinues = animation.update();

  if (!animationContinues && state == InProgress)
    performAction();
}

void InteractionAction::performAction()
{
  qDebug() << "InteractionAction::performAction";
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
    else if (actionName == "use" && typeName == "InventoryItem")
      level->pickUpItem(character, reinterpret_cast<InventoryItem*>(target));
    state = Interrupted;
  }
  else
    state = Done;
}

int InteractionAction::getApCost() const
{
  return 2;
}
