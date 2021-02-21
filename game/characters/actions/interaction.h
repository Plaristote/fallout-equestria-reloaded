#ifndef  INTERACTION_ACTION_H
# define INTERACTION_ACTION_H

# include "base.h"

class InteractionAction : public ActionBase
{
public:
  InteractionAction(Character* character, DynamicObject* target, QString actionName) : ActionBase(character), target(target), actionName(actionName)
  {
  }

  int  getApCost() const override;
  bool trigger() override;

protected:
  DynamicObject* target;
  QString        actionName;
};

#endif
