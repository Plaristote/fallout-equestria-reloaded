#ifndef SKILL_USE_ACTION_H
#define SKILL_USE_ACTION_H

# include "base.h"
# include "game/animationSequence/animationsequence.h"

class SkillAction : public ActionBase
{
public:
  SkillAction(Character* character, DynamicObject* target, const QString& skillName) : ActionBase(character), target(target), skillName(skillName)
  {
  }

  int getApCost() const override;
  bool trigger() override;
  void update() override;

protected:
  void performAction();
  bool performSteal();

  DynamicObject*    target;
  QString           skillName;
  AnimationSequence animation;
};

#endif // SKILLACTION_H
