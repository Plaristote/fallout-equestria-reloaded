#include "objectanimationpart.h"
#include <QJSValue>
#include "game/dynamicobject.h"

void ObjectAnimationPart::initialize(QJSValue& value)
{
  QObject* rawTarget = value.property("object").toQObject();

  if (rawTarget)
  {
    QJSValue jsPostAnimationName = value.property("afterAnimation");

    initialize(
      reinterpret_cast<DynamicObject*>(value.property("object").toQObject()),
      value.property("animation").toString(),
      jsPostAnimationName.isString() ? jsPostAnimationName.toString() : "idle"
    );
  }
  else
    over = true;
}

void ObjectAnimationPart::initialize(DynamicObject* target, const QString& animationName, const QString& postAnimationName)
{
  this->target            = target;
  this->animationName     = animationName;
  this->postAnimationName = postAnimationName;
  connect(target, &Sprite::animationFinished, this, &ObjectAnimationPart::onAnimationFinished);
}

void ObjectAnimationPart::start()
{
  if (target)
  {
    target->setAnimation(animationName);
    if (!target->isAnimated())
      onAnimationFinished();
  }
  else
    qDebug() << "/!\\ Invalid object set for ObjectAnimationPart";
}

void ObjectAnimationPart::finish()
{
  if (postAnimationName.length())
    target->setAnimation(postAnimationName);
}

void ObjectAnimationPart::onAnimationFinished()
{
  over = true;
  finish();
}
