#include "movement.h"
#include "game.h"

bool MovementAction::trigger()
{
  auto* level = Game::get()->getLevel();
  auto* grid  = level->getFloorGrid(character->getCurrentFloor());

  if (grid->findPath(character->getPoint(), target, character->rcurrentPath(), character))
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

int MovementAction::pathApCost(const QList<Point>& path) const
{
  int ap = 0;
  auto lastFloor = static_cast<unsigned char>(character->getCurrentFloor());

  for (const Point& point : path)
  {
    ap += (point.z == lastFloor ? 1 : 3);
    lastFloor = point.z;
  }
  return ap;
}

int MovementAction::getApCost() const
{
  return pathApCost(character->getCurrentPath());
}

bool MovementAction::canMakeNextMovement() const
{
  if (character->getCurrentPath().length() > 0)
  {
    auto*  level        = Game::get()->getLevel();

    if (level->isInCombat(character))
    {
      Point  nextPosition = character->rcurrentPath().front();
      auto*  grid         = level->getFloorGrid(character->getCurrentFloor());
      auto*  currentCase  = grid->getGridCase(character->getPoint());
      int    ap = 1;

      if (currentCase->position.z != nextPosition.z)
        ap = 3;
      return character->getActionPoints() >= ap;
    }
    return true;
  }
  return false;
}

void MovementAction::triggerNextMovement()
{
  if (character->getCurrentPath().length() > 0)
  {
    Point  nextPosition = character->rcurrentPath().front();
    auto*  level        = Game::get()->getLevel();
    auto*  grid         = level->getFloorGrid(character->getCurrentFloor());
    auto*  currentCase  = grid->getGridCase(character->getPoint());
    auto*  nextCase     = grid->getGridCase(nextPosition);
    auto*  connection   = currentCase ? currentCase->connectionWith(nextCase) : nullptr;
    int    ap           = 1;

    if (connection && nextCase && (!nextCase->occupied || nextCase->occupant == character))
    {
      if (currentCase->position.z != nextPosition.z)
        ap = 3;
      if (connection->goThrough(character) && character->useActionPoints(ap, "movement"))
        character->moveTo(nextPosition);
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
  auto* grid  = level->getFloorGrid(character->getCurrentFloor());

  if (grid)
    grid->triggerZone(character, character->getPosition());
}
