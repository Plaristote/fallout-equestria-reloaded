#ifndef OBJECTANIMATIONPART_H
#define OBJECTANIMATIONPART_H

#include "ianimationpart.h"
#include <QString>
#include <QObject>

class DynamicObject;

class ObjectAnimationPart : public IAnimationPart
{
  DynamicObject* target;
  QString        animationName, postAnimationName;
  bool           over = false;
  QMetaObject::Connection eventListener;
public:
  ~ObjectAnimationPart() override;
  static bool matches(const QJSValue&);
  void initialize(QJSValue& value) override;
  void initialize(DynamicObject*, const QString& animationName, const QString& postAnimationName = "");
  void start() override;
  void finish() override;
  bool isOver() const override { return over; }
  void onAnimationFinished();
};

#endif // OBJECTANIMATIONPART_H
