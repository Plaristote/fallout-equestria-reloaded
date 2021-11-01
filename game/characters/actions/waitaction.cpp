#include "waitaction.h"
#include "game.h"
#include "game/timermanager.h"

WaitAction::WaitAction(Character* character, unsigned int interval) : ActionBase(character), interval(interval)
{
}

bool WaitAction::trigger()
{
  DateTime dateTime = Game::get()->getTimeManager()->getDateTime();

  endsAt = dateTime + DateTime::Seconds(interval);
  return true;
}

void WaitAction::update()
{
  DateTime dateTime = Game::get()->getTimeManager()->getDateTime();

  if (dateTime >= endsAt)
    state = Done;
}
