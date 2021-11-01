#include "look.h"

LookAction::LookAction(Character* character, QPoint target) : ActionBase(character), target(target)
{

}

void LookAction::update()
{
  state = Done;
}

bool LookAction::trigger()
{
  character->lookTo(target);
  return true;
}
