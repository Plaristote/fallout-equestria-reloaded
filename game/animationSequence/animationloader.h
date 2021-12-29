#ifndef  ANIMATIONLOADER_H
# define ANIMATIONLOADER_H

# include "groupanimationpart.h"
# include "objectanimationpart.h"
# include "spriteanimationpart.h"
# include <QJSValue>
# include <QDebug>

template<typename ANIMATION_GROUP>
void loadAnimationGroup(ANIMATION_GROUP& group, QJSValue& value)
{
  unsigned int length = value.property("length").toUInt();

  for (unsigned int i = 0 ; i < length ; ++i)
  {
    QJSValue animationDescriptor = value.property(i);
    QString  type = animationDescriptor.property("type").toString();

    if (animationDescriptor.isArray())
    {
      auto* part = new GroupAnimationPart;

      part->initialize(animationDescriptor);
      group.addAnimationPart(part);
    }
    else if (type == "Sprite")
    {
      auto* part = new SpriteAnimationPart;

      part->initialize(animationDescriptor);
      group.addAnimationPart(part);
    }
    else if (type == "Animation")
    {
      auto* part = new ObjectAnimationPart;

      part->initialize(animationDescriptor);
      group.addAnimationPart(part);
    }
    else
      qDebug() << "ActionAnimation: unknown animation type" << type;
  }
}

#endif // ANIMATIONLOADER_H
