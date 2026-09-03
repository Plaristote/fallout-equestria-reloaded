#include "sliding.h"
#include "game/leveltask.h"
#define TIMEOUT_DURATION_MS 10000

SlidingAction::SlidingAction(Character* character, QPoint target) : ActionBase(character), target(target)
{
}

bool SlidingAction::trigger()
{
  QPoint coordinates = getTargetCoordinates();
  QPoint origin = character->getPosition();

  if (origin != coordinates)
  {
    auto* grid = LevelTask::get()->getFloorGrid(character->getCurrentFloor());

    character->moveToCoordinates(coordinates);
    grid->moveObject(character, target.x(), target.y());
    character->lookTo(origin);
  }
  character->setFallAnimation();
  timer.restart();
  return true;
}

void SlidingAction::update()
{
  bool finished = !character->isSpriteMoving() && !animationRunning();

  if (finished || timer.hasExpired(TIMEOUT_DURATION_MS))
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
  auto* grid = LevelTask::get()->getFloorGrid(character->getCurrentFloor());

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
  auto* level = LevelTask::get();

  return level->getRenderPositionForTile(target.x(), target.y());
}
