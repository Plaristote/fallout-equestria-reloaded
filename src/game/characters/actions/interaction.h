#ifndef  INTERACTION_ACTION_H
# define INTERACTION_ACTION_H

# include "base.h"
# include "game/animationSequence/animationsequence.h"

class InteractionAction : public ActionBase
{
public:
  InteractionAction(Character* character, DynamicObject* target, QString actionName) : ActionBase(character), target(target), actionName(actionName)
  {
  }

  int  getApCost() const override;
  bool trigger() override;
  void update() override;

protected:
  void performAction();

  DynamicObject*    target;
  QString           actionName;
  AnimationSequence animation;
};

#endif
