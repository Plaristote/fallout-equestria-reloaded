#include "objectanimationpart.h"
#include <QJSValue>
#include "game/dynamicobject.h"

void ObjectAnimationPart::initialize(QJSValue& value)
{
  QObject* rawTarget = value.property("object").toQObject();

  if (rawTarget)
  {
    QJSValue jsPostAnimationName = value.property("afterAnimation");

    target = reinterpret_cast<DynamicObject*>(value.property("object").toQObject());
    animationName = value.property("animation").toString();
    if (jsPostAnimationName.isString())
      postAnimationName = jsPostAnimationName.toString();
    else
      postAnimationName = "idle";
    connect(target, &Sprite::animationFinished, this, &ObjectAnimationPart::onAnimationFinished);
  }
  else
  {
    qDebug() << "ObjectAnimation: attempted to initialize animation without an object";
    over = true;
  }
}

void ObjectAnimationPart::start()
{
  target->setAnimation(animationName);
}

void ObjectAnimationPart::finish()
{
  if (postAnimationName.length())
    target->setAnimation(postAnimationName);
}
