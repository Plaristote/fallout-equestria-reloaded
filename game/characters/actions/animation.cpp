#include "animation.h"
#include "game/animationSequence/objectanimationpart.h"

AnimationAction::AnimationAction(Character* c, const QString& animationName, const QString& postAnimationName) : AnimatedAction(c)
{
  auto* animationPart = new ObjectAnimationPart;

  animationPart->initialize(character, animationName, postAnimationName);
  animation.addAnimationPart(animationPart);
}

AnimationAction::AnimationAction(Character* c, QJSValue animationData) : AnimatedAction(c)
{
  animation.initialize(animationData);
}

bool AnimationAction::trigger()
{
  animation.start();
  state = InProgress;
  return true;
}

void AnimationAction::performAction()
{
  state = Done;
}
