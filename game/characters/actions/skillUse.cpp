#include "skillUse.h"
#include "game.h"
#include "game/leveltask.h"
#include "game/animationSequence/objectanimationpart.h"

bool SkillAction::trigger()
{
  bool instantUseSkill = skillName == "sneak";

  character->lookTo(target->getPosition());
  if (instantUseSkill)
    performAction();
  else
  {
    auto* animationPart = new ObjectAnimationPart;

    animationPart->initialize(character, "use", "idle");
    animation.addAnimationPart(animationPart);
    animation.start();
    state = InProgress;
  }
  return state != Interrupted;
}

void SkillAction::update()
{
  bool animationContinues = animation.update();

  if (!animationContinues && state == InProgress)
    performAction();
}

void SkillAction::performAction()
{
  auto* level = Game::get()->getLevel();
  bool  isInCombat = level && level->isInCombat(character);
  bool  success = false;

  if (skillName == "steal")
    success = performSteal();
  else if (!isInCombat || getApCost() <= character->getActionPoints())
    success = target->triggerSkillUse(character, skillName);
  else if (character == level->getPlayer())
    emit level->displayConsoleMessage("Not enough action points.");
  state = success ? Done : Interrupted;
}

bool SkillAction::performSteal()
{
  auto* level = Game::get()->getLevel();
  bool  isInCombat = level && level->isInCombat(character);

  if (!isInCombat)
  {
    QString type = target->getObjectType();

    if (type == "StorageObject" || type == "Character")
    {
      level->initializeLooting(reinterpret_cast<StorageObject*>(target));
      return true;
    }
    else
      emit level->displayConsoleMessage("Invalid target.");
  }
  else
    emit level->displayConsoleMessage("Not available in combat.");
  return false;
}

int SkillAction::getApCost() const
{
  if (skillName == "sneak" || skillName == "medicine")
    return 0;
  return 2;
}
