#ifndef  ANIMATIONLOADER_H
# define ANIMATIONLOADER_H

# include "groupanimationpart.h"
# include "objectanimationpart.h"
# include "spriteanimationpart.h"
# include "soundanimationpart.h"
# include <QJSValue>
# include <QDebug>

template<typename PART, typename ANIMATION_GROUP>
void addAnimationToGroup(ANIMATION_GROUP& group, QJSValue& animationDescriptor)
{
  auto* part = new PART;

  part->initialize(animationDescriptor);
  group.addAnimationPart(part);
}

template<typename ANIMATION_GROUP, typename CURRENT_PART, typename ...PARTS>
void recursiveAnimationPartLoading(ANIMATION_GROUP& group, QJSValue& animationDescriptor)
{
  if (CURRENT_PART::matches(animationDescriptor))
    addAnimationToGroup<CURRENT_PART>(group, animationDescriptor);
  else
    recursiveAnimationPartLoading<ANIMATION_GROUP, PARTS...>(group, animationDescriptor);
}

template<typename ANIMATION_GROUP>
void recursiveAnimationPartLoading(ANIMATION_GROUP&, QJSValue& animationDescriptor)
{
  qDebug() << "ActionAnimation: unknown animation type" << animationDescriptor.property("type").toString();
}

template<typename ANIMATION_GROUP>
void loadAnimationGroup(ANIMATION_GROUP& group, QJSValue& value)
{
  unsigned int length = value.property("length").toUInt();

  for (unsigned int i = 0 ; i < length ; ++i)
  {
    QJSValue animationDescriptor = value.property(i);
    QString  type = animationDescriptor.property("type").toString();

    recursiveAnimationPartLoading<ANIMATION_GROUP,
      SpriteAnimationPart,
      ObjectAnimationPart,
      SoundAnimationPart,
      GroupAnimationPart
    >(group, animationDescriptor);
  }
}

#endif // ANIMATIONLOADER_H
