#include "soundmanager.h"
#include "globals.h"
#include "game.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSoundEffect>
#include <QSettings>
#include <cmath>

SoundManager* SoundManager::_global_ptr = nullptr;
static const QString volumeOption = "audio/soundVolume";

SoundManager::SoundManager(QObject *parent) : QObject(parent)
{
  _global_ptr = this;
  timer.setInterval(1000);
  connect(&timer, &QTimer::timeout, this, &SoundManager::update);
}

SoundManager::~SoundManager()
{
  stop();
}

void SoundManager::initialize()
{
  if (!initialized)
  {
    QFile file(ASSETS_PATH + "audio.json");

    if (file.open(QIODevice::ReadOnly))
    {
      const QJsonObject data = QJsonDocument::fromJson(file.readAll()).object();
      const QStringList keys = data.keys();

      file.close();
      for (const QString& key : keys)
        soundLibrary.insert(key, QUrl::fromLocalFile(SOUNDS_PATH + data[key].toString()));
      initialized = true;
    }
  }
}

void SoundManager::stop()
{
  auto it = sounds.begin();

  while (it != sounds.end())
  {
    QSharedPointer<QSoundEffect> sound = *it;

    sound->stop();
    it = sounds.erase(it);
  }
}

void SoundManager::setDefaultVolume(int value)
{
  QSettings().setValue(volumeOption, value);
  emit defaultVolumeChanged();
}

int SoundManager::getDefaultVolume() const
{
  return QSettings().value(volumeOption, 100).toInt();
}

void SoundManager::update()
{
  for (auto it = sounds.begin() ; it != sounds.end() ;)
  {
    QSharedPointer<QSoundEffect> sound = *it;

    if (sound->status() != QSoundEffect::Loading && !sound->isPlaying())
      it = sounds.erase(it);
    else
      it++;
  }
  if (!sounds.size())
    timer.stop();
}

void SoundManager::play(const QString& name, qreal volume)
{
  if (soundLibrary.contains(name))
  {
    auto volumeLevel = QSettings().value(volumeOption, 100).toInt();
    auto sound = QSharedPointer<QSoundEffect>(new QSoundEffect, &QSoundEffect::deleteLater);

    sound->setSource(soundLibrary[name]);
    sound->setLoopCount(1);
    sound->setVolume(volume * volumeLevel / 100);
    sound->play();
    sounds.push_back(sound);
    timer.start();
  }
}

void SoundManager::play(const DynamicObject* object, const QString& name, qreal volume)
{
  const Character* player = Game::get()->getPlayer();

  if (player->getCurrentFloor() == object->getCurrentFloor())
  {
    const float distance = player->getDistance(object);
    const int   factor   = static_cast<int>(std::floor(distance)) / 6;

    if (factor > 1)
      volume /= factor;
    play(name, volume);
  }
}
