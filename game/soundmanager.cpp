#include "soundmanager.h"
#include "globals.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSoundEffect>

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
    auto sound = QSharedPointer<QSoundEffect>(new QSoundEffect);

    sound->setSource(soundLibrary[name]);
    sound->setLoopCount(1);
    sound->setVolume(volume);
    sound->play();
    sounds.push_back(sound);
  }
}
