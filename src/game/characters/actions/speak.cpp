#include "speak.h"
#include "game/leveltask.h"

SpeakAction::SpeakAction(Character* character, const QString& content, unsigned int duration, const QString& color)
  : ActionBase(character), content(content), color(color), duration(duration)
{
}

bool SpeakAction::trigger()
{
  auto* level = LevelTask::get();

  level->addTextBubble(character, content, duration, color);
  return true;
}

void SpeakAction::update()
{
  state = Done;
}
