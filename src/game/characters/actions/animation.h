#ifndef ANIMATIONACTION_H
#define ANIMATIONACTION_H

#include "animatedaction.h"

class AnimationAction : public AnimatedAction
{
public:
  AnimationAction(Character*, const QString& animationName, const QString& postAnimationName);
  AnimationAction(Character*, QJSValue animationData);

  int      getApCost() const override { return 0; }
  bool     trigger() override;
protected:
  void     performAction() override;
  void     lookAtTarget() override {}
  QJSValue getDefaultCallback() override { return QJSValue(); }
};

#endif // ANIMATIONACTION_H
