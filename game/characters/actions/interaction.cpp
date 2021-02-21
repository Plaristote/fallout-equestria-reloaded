#include "interaction.h"
#include "game.h"

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
