#include "animatedaction.h"
#include "game/animationSequence/objectanimationpart.h"

bool AnimatedAction::triggerAnimation(QJSValue data)
{
  lookAtTarget();
  if (data.isObject())
  {
    QJSValue animationSteps = data.property("steps");

    callback = data.property("callback");
    animation.initialize(animationSteps);
    animation.start();
    state = InProgress;
  }
  else if (data.toBool())
  {
    auto* animationPart = new ObjectAnimationPart;

    callback = getDefaultCallback();
    animationPart->initialize(character, "use", "idle");
    animation.addAnimationPart(animationPart);
    animation.start();
    state = InProgress;
  }
  else
    state = Interrupted;
  return state != Interrupted;
}

void AnimatedAction::update()
{
  bool animationContinues = animation.update();

  if (!animationContinues && state == InProgress)
    performAction();
}

bool AnimatedAction::canInterrupt() const
{
  return !(state == InProgress && animation.isRunning());
}
