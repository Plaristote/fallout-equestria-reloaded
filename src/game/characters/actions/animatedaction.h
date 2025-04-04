#ifndef  ANIMATEDACTION_H
# define ANIMATEDACTION_H

# include "base.h"
# include "game/animationSequence/animationsequence.h"

class AnimatedAction : public ActionBase
{
public:
  AnimatedAction(Character* c) : ActionBase(c) {}

  virtual void update() override;
  virtual bool canInterrupt() const override;

protected:
  bool triggerAnimation(QJSValue);
  virtual void     performAction() = 0;
  virtual void     lookAtTarget() = 0;
  virtual QJSValue getDefaultCallback() = 0;

  AnimationSequence animation;
  QJSValue          callback;
};

#endif // ANIMATEDACTION_H
