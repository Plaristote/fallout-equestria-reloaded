#include "movement.h"
#include "game.h"

bool MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();
  auto* grid  = level->getGrid();

  if (grid->findPath(character->getPosition(), target, character->rcurrentPath(), character))
    state = InProgress;
  else
    state = Interrupted;
  firstRound = true;
  return state == InProgress;
}

void MovementAction::update()
{
  if (!character->isSpriteMoving())
  {
    if (firstRound)
      firstRound = false;
    else
      onMovementFinished();
    triggerNextMovement();
  }
}

int MovementAction::getApCost() const
{
  return character->getCurrentPath().size();
}

void MovementAction::triggerNextMovement()
{
  if (character->getCurrentPath().length() > 0)
  {
    QPoint nextPosition = character->rcurrentPath().front();
    auto*  level        = Game::get()->getLevel();
    auto*  grid         = level->getGrid();
    auto*  currentCase  = grid->getGridCase(character->getPosition());
    auto*  nextCase     = grid->getGridCase(nextPosition);
    auto*  connection   = currentCase ? currentCase->connectionWith(nextCase) : nullptr;

    if (nextCase && (!nextCase->occupied || nextCase->occupant == character))
    {
      if (connection->goThrough(character) && character->useActionPoints(1, "movement"))
        character->moveTo(nextPosition.x(), nextPosition.y());
      else
        state = Interrupted;
      character->rcurrentPath().pop_front();
    }
    else
    {
      character->rcurrentPath().clear();
      state = Interrupted;
    }
  }
  else
    state = Done;
  if (state != InProgress)
    interrupt();
}

void MovementAction::interrupt()
{
  character->setAnimation("idle");
  onMovementFinished();
}

void MovementAction::onMovementFinished()
{
  auto* level = Game::get()->getLevel();
  auto* grid  = level->getGrid();

  grid->triggerZone(character, character->getPosition());
}
