#include "speak.h"
#include "game.h"
#include "game/leveltask.h"

SpeakAction::SpeakAction(Character* character, const QString& content, unsigned int duration, const QString& color)
  : ActionBase(character), content(content), duration(duration), color(color)
{
}

bool SpeakAction::trigger()
{
  auto* level = Game::get()->getLevel();

  level->addTextBubble(character, content, duration, color);
  return true;
}

void SpeakAction::update()
{
  state = Done;
}
