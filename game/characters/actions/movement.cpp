#include "movement.h"
#include "game.h"

bool MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();

  return level->moveTo(character, target);
}

bool MovementAction::isOver()
{
  return character->getPosition() == target && !character->isMoving();
}

int MovementAction::getApCost() const
{
  return character->getCurrentPath().size();
}
