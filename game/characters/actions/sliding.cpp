#include "sliding.h"
#include "game.h"

SlidingAction::SlidingAction(Character* character, QPoint target) : ActionBase(character), target(target)
{
}

bool SlidingAction::trigger()
{
  QPoint coordinates = getTargetCoordinates();
  QPoint origin = character->getPosition();

  if (origin != coordinates)
  {
    auto* grid = Game::get()->getLevel()->getFloorGrid(character->getCurrentFloor());

    character->moveToCoordinates(coordinates);
    grid->moveObject(character, target.x(), target.y());
    character->lookTo(origin);
  }
  character->setFallAnimation();
  return true;
}

void SlidingAction::update()
{
  if (!character->isSpriteMoving() && !animationRunning())
  {
    state = Done;
    onFinished();
  }
}

void SlidingAction::interrupt()
{
  character->setRenderPosition(getTargetCoordinates());
  onFinished();
}

void SlidingAction::onFinished()
{
  auto* grid = Game::get()->getLevel()->getFloorGrid(character->getCurrentFloor());

  if (!character->isUnconscious())
    character->wakeUp();
  grid->triggerZone(character, target.x(), target.y());
}

bool SlidingAction::animationRunning() const
{
  return character->isAnimated() && character->getAnimation().startsWith("fall");
}

QPoint SlidingAction::getTargetCoordinates() const
{
  auto* level = Game::get()->getLevel();

  return level->getRenderPositionForTile(target.x(), target.y());
}
