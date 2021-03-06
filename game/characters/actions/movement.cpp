#include "movement.h"
#include "game.h"

bool MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();

  state = InProgress;
  return level->moveTo(character, target);
}

void MovementAction::update()
{
  if (character->getPosition() == target && !character->isMoving())
    state = Done;
}

int MovementAction::getApCost() const
{
  return character->getCurrentPath().size();
}
