#ifndef SOUNDANIMATIONPART_H
#define SOUNDANIMATIONPART_H

#include "ianimationpart.h"
#include <QString>

class DynamicObject;

class SoundAnimationPart : public IAnimationPart
{
  DynamicObject* object;
  QString        sound;
  qreal          volume;
public:
  SoundAnimationPart();

  static bool matches(const QJSValue&);
  virtual void initialize(QJSValue&);
  virtual void start();
  virtual bool isOver() const;
};

#endif // SOUNDANIMATIONPART_H
