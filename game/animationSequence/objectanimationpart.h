#ifndef OBJECTANIMATIONPART_H
#define OBJECTANIMATIONPART_H

#include "ianimationpart.h"

class DynamicObject;

class ObjectAnimationPart : public IAnimationPart
{
  Q_OBJECT

  DynamicObject* target;
  QString        animationName, postAnimationName;
  bool           over = false;
public:
  void initialize(QJSValue& value) override;
  void start() override;
  void finish() override;
  bool isOver() const { return over; }

private slots:
  void onAnimationFinished() { over = true; }
};

#endif // OBJECTANIMATIONPART_H
