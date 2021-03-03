#include "skillUse.h"

bool SkillAction::trigger()
{
  qDebug() << "SkillAction::trigger: triggered" << skillName;
  qDebug() << "TODO implements skill use scripting";
  return true;
}

int SkillAction::getApCost() const
{
  if (skillName == "sneak")
    return 0;
  return 2;
}
