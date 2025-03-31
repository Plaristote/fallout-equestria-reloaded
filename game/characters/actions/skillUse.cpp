#include "skillUse.h"
#include "game.h"
#include "game/leveltask.h"
#include "game/animationSequence/objectanimationpart.h"
#include "i18n.h"

bool SkillAction::trigger()
{
  bool instantUseSkill = skillName == "sneak";

  if (target)
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
  auto* i18n = I18n::get();
  auto* level = Game::get()->getLevel();
  bool  isInCombat = level && level->isInCombat(character);
  bool  success = false;
  std::function<bool()> fallback;

  if (skillName == "steal")
    fallback = std::bind(&SkillAction::performSteal, this);
  if (skillName == "sneak")
    success = level->useSneak(character);
  else if (!isInCombat || getApCost() <= character->getActionPoints())
  {
    success = target->triggerSkillUse(character, skillName, fallback);
    character->useActionPoints(getApCost(), "skill-" + skillName);
  }
  else if (character == level->getPlayer())
    emit level->displayConsoleMessage(i18n->t("messages.not-enough-ap"));
  state = success ? Done : Interrupted;
}

bool SkillAction::performSteal()
{
  auto* i18n = I18n::get();
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
      emit level->displayConsoleMessage(i18n->t("messages.invalid-target"));
  }
  else
    emit level->displayConsoleMessage(i18n->t("messages.not-available-in-combat"));
  return false;
}

int SkillAction::getApCost() const
{
  if (skillName == "sneak" || skillName == "medicine")
    return 0;
  return 4;
}
