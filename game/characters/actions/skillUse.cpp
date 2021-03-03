#include "skillUse.h"
#include "game.h"
#include "game/leveltask.h"

bool SkillAction::trigger()
{
  auto* level = Game::get()->getLevel();
  bool  isInCombat = level && level->isInCombat(character);

  if (!isInCombat || getApCost() <= character->getActionPoints())
    return target->triggerSkillUse(character, skillName);
  else if (character == level->getPlayer())
    emit level->displayConsoleMessage("Not enough action points.");
  return false;
}

int SkillAction::getApCost() const
{
  if (skillName == "sneak" || skillName == "medicine")
    return 0;
  return 2;
}
