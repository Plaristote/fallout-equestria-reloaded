#include "soundmanager.h"
#include "globals.h"
#include "game.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSoundEffect>
#include <QSettings>
#include <cmath>

SoundManager::SoundManager(QObject *parent) : QObject(parent)
{
  QFile file(ASSETS_PATH + "audio.json");

  if (file.open(QIODevice::ReadOnly))
  {
    const QJsonObject data = QJsonDocument::fromJson(file.readAll()).object();
    const QStringList keys = data.keys();

    file.close();
    for (const QString& key : keys)
       soundLibrary.insert(key, QUrl::fromLocalFile(SOUNDS_PATH + data[key].toString()));
  }
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
}

void SoundManager::play(const QString& name, qreal volume)
{
  if (soundLibrary.contains(name))
  {
    auto volumeLevel = QSettings().value("audio/volume", 100).toInt();
    auto sound = QSharedPointer<QSoundEffect>(new QSoundEffect);

    sound->setSource(soundLibrary[name]);
    sound->setLoopCount(1);
    sound->setVolume(volume * volumeLevel / 100);
    sound->play();
    sounds.push_back(sound);
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
