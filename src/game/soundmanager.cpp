#include "soundmanager.h"
#include "globals.h"
#include "game.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSoundEffect>
#include <QSettings>
#include <cmath>

SoundWorker::SoundWorker()
{
  timer.setInterval(1000);
  timer.setSingleShot(false);
  connect(&timer, &QTimer::timeout, this, &SoundWorker::update);
}

SoundWorker::~SoundWorker()
{
  timer.stop();
}

void SoundWorker::update()
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

void SoundWorker::playSound(const QUrl& url, float volume)
{
  auto sound = QSharedPointer<QSoundEffect>(new QSoundEffect, &QSoundEffect::deleteLater);

  sound->setSource(url);
  sound->setLoopCount(1);
  sound->setVolume(volume);
  sound->play();
  sounds.push_back(sound);
  timer.start();
}

SoundManager* SoundManager::_global_ptr = nullptr;
static const QString volumeOption = "audio/soundVolume";

SoundManager::SoundManager(QObject *parent) : QObject(parent)
{
  _global_ptr = this;
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
  start();
}

void SoundManager::start()
{
  if (!workerThread)
  {
    SoundWorker* worker = new SoundWorker();
    workerThread = new QThread(this);

    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
    connect(this, &SoundManager::playSound, worker, &SoundWorker::playSound, Qt::QueuedConnection);
    workerThread->start();
  }
}

void SoundManager::stop()
{
  if (workerThread)
  {
    workerThread->quit();
    workerThread = nullptr;
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

void SoundManager::play(const QString& name, qreal volume)
{
  if (soundLibrary.contains(name))
  {
    auto volumeLevel = QSettings().value(volumeOption, 100).toInt();

    emit playSound(soundLibrary[name], volume * volumeLevel / 100);
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
