#ifndef SKILL_USE_ACTION_H
#define SKILL_USE_ACTION_H

# include "base.h"

class SkillAction : public ActionBase
{
public:
  SkillAction(Character* character, DynamicObject* target, const QString& skillName) : ActionBase(character), target(target), skillName(skillName)
  {
  }

  int getApCost() const override;
  bool trigger() override;

protected:
  DynamicObject* target;
  QString        skillName;
};

#endif // SKILLACTION_H
