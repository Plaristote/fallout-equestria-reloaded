#include "objectanimationpart.h"
#include <QJSValue>
#include <QDebug>
#include "game/dynamicobject.h"

ObjectAnimationPart::~ObjectAnimationPart()
{
  QObject::disconnect(eventListener);
}

bool ObjectAnimationPart::matches(const QJSValue& descriptor)
{
  return descriptor.property("type").toString() == "Animation";
}

void ObjectAnimationPart::initialize(QJSValue& value)
{
  QObject* rawTarget = value.property("object").toQObject();

  if (rawTarget)
  {
    QJSValue jsPostAnimationName = value.property("afterAnimation");

    initialize(
      reinterpret_cast<DynamicObject*>(rawTarget),
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
  eventListener = QObject::connect(target, &Sprite::animationFinished, std::bind(&ObjectAnimationPart::onAnimationFinished, this));
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
