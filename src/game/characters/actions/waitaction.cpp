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
  Game* game = Game::get();
  auto* level = game->getLevel();
  DateTime dateTime = game->getTimeManager()->getDateTime();

  if (dateTime >= endsAt)
    state = Done;
  else if (level && level->isInCombat(character))
    level->passTurn(character);
}
