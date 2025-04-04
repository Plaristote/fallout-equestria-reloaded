#include "soundanimationpart.h"
#include "game.h"
#include <QJSValue>

SoundAnimationPart::SoundAnimationPart() : object(nullptr), volume(1.0)
{
}

bool SoundAnimationPart::matches(const QJSValue& descriptor)
{
  return descriptor.property("type").toString() == "Sound";
}

void SoundAnimationPart::initialize(QJSValue& value)
{
  sound = value.property("sound").toString();
  if (value.property("volume").isNumber())
    volume = value.property("volume").toNumber();
  if (value.property("object").isQObject())
    object = reinterpret_cast<DynamicObject*>(value.property("object").toQObject());
}

void SoundAnimationPart::start()
{
  SoundManager* sounds = Game::get()->getSoundManager();

  qDebug() << "start sound animation" << object << sound << volume;
  if (object)
    sounds->play(object, sound, volume);
  else
    sounds->play(sound, volume);
}

bool SoundAnimationPart::isOver() const
{
  return true;
}
