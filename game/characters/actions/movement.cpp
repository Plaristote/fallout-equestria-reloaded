#include "movement.h"
#include "game.h"

bool MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();
  auto* grid  = level->getGrid();

  if (grid->findPath(character->getPosition(), target, character->rcurrentPath()))
    state = InProgress;
  else
    state = Interrupted;
  return state == InProgress;
}

void MovementAction::update()
{
  if (!character->isSpriteMoving())
    triggerNextMovement();
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
    auto*  nextCase     = grid->getGridCase(nextPosition.x(), nextPosition.y());

    qDebug() << "Next position: " << nextPosition;
    qDebug() << "Noow position: " << character->getPosition();
    if (nextCase && (!nextCase->occupied || nextCase->occupant == character))
    {
      if (!level->isInCombat(character) || character->useActionPoints(1, "movement"))
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
}
