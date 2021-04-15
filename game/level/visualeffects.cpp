 #include "visualeffects.h"

VisualEffectsComponent::VisualEffectsComponent(QObject* parent) : ParentType(parent)
{

}

VisualEffectsComponent::~VisualEffectsComponent()
{
  for (auto it = runningAnimations.begin() ; it != runningAnimations.end() ; ++it)
    delete it->first;
}

void VisualEffectsComponent::registerVisualEffect(Sprite* sprite)
{
  visualEffects << sprite;
  emit visualEffectsChanged();
}

void VisualEffectsComponent::unregisterVisualEffect(Sprite* sprite)
{
  visualEffects.removeAll(sprite);
  emit visualEffectsChanged();
}

void VisualEffectsComponent::updateVisualEffects(qint64 delta)
{
  updateSprites(delta);
  updateRunningAnimations();
}

void VisualEffectsComponent::updateSprites(qint64 delta)
{
  for (Sprite* sprite : qAsConst(visualEffects))
    sprite->update(delta);
}

void VisualEffectsComponent::updateRunningAnimations()
{
  QVector<QJSValue> callbacks;

  callbacks.reserve(runningAnimations.length());
  for (auto it = runningAnimations.begin() ; it != runningAnimations.end() ;)
  {
    AnimationSequence* animation = it->first;

    if (animation->update())
      ++it;
    else
    {
      QJSValue callback = it->second;

      it = runningAnimations.erase(it);
      delete animation;
      if (callback.isCallable())
        callbacks << callback;
    }
  }
  for (QJSValue callback : qAsConst(callbacks))
    callback.call();
}

void VisualEffectsComponent::addAnimationSequence(QJSValue descriptor)
{
  if (descriptor.isObject())
  {
    QJSValue animationSteps = descriptor.property("steps");

    if (animationSteps.isArray())
    {
      AnimationSequence* animation = new AnimationSequence;
      QJSValue callback = descriptor.property("callback");

      animation->initialize(animationSteps);
      addAnimationSequence(animation, callback);
    }
    else
      qDebug() << "VisualEffectsComponent::addAnimationSequence: missing `steps` array";
  }
  else
    qDebug() << "VisualEffectsComponent::addAnimationSequence: invalid parameter";
}

void VisualEffectsComponent::addAnimationSequence(AnimationSequence* animation, QJSValue callback)
{
  animation->start();
  runningAnimations << QPair<AnimationSequence*, QJSValue>(animation, callback);
}
