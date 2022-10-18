#include "musicmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QSettings>
#include <QDebug>

using namespace std;

MusicManager* MusicManager::_global_ptr = nullptr;

MusicManager::MusicManager(QObject* parent) : QObject(parent)
{
  audioManager = new QMediaPlayer(this);
  _global_ptr  = this;
  fadingOut    = false;
  loadDataTree();
  fadingTimer.setInterval(50);
  fadingTimer.setSingleShot(false);
  connect(&fadingTimer, &QTimer::timeout,            this, &MusicManager::fadeVolume);
  connect(audioManager, &QMediaPlayer::stateChanged, this, &MusicManager::onStateChanged);
  connect(this, &MusicManager::defaultVolumeChanged, [this]() { setVolumeToDefault(); });
  setVolumeToDefault();
}

MusicManager::~MusicManager()
{
  _global_ptr = nullptr;
}

void MusicManager::loadDataTree()
{
  QFile musicSource(ASSETS_PATH + "musics.json");

  if (musicSource.open(QIODevice::ReadOnly))
  {
    dataTree = QJsonDocument::fromJson(musicSource.readAll()).object();
    musicSource.close();
  }
  else
    qDebug() << "Missing musics.json";
}

void MusicManager::pause(bool paused)
{
  if (paused)
    audioManager->pause();
  else
    audioManager->play();
}

void MusicManager::play(const QString& category)
{
  if (currentCategory == category) return ;
  currentCategory = category;
  playNext();
}

void MusicManager::play(const QString& scategory, const QString& name)
{
  QJsonValue category = dataTree[scategory].toObject();
  QJsonValue track;

  if (category.isUndefined()) return ;
  track = category[name];
  if (track.isUndefined())    return ;
  if (audioManager->state() == QMediaPlayer::PlayingState)
  {
    nextTrack = track.toString();
    fadeOut();
  }
  else
    startTrack(track.toString());
}

QStringList MusicManager::potentialSuccessors(void) const
{
  const QJsonObject category = dataTree[currentCategory].toObject();
  QStringList keys;

  for (auto it = category.begin() ; it != category.end() ; ++it)
  {
    if (it.value() != currentTrack)
      keys << it.key();
  }
  return keys;
}

void MusicManager::playNext(void)
{
  const QJsonObject    category = dataTree[currentCategory].toObject();
  const QStringList    keys     = potentialSuccessors();
  const unsigned short max      = static_cast<unsigned short>(keys.length());

  qDebug() << "MusicManager::playNext in category" << currentCategory << ", options:" << keys;
  if (max == 0)
  {
    if (currentTrack != "")
      startTrack(currentTrack);
  }
  else
    play(currentCategory, keys.at(rand() % max));
}

void MusicManager::startTrack(const QString& filename)
{
  currentTrack = filename;
  nextTrack    = currentTrack;
  if (audioManager)
  {
    disconnect(audioManager, &QMediaPlayer::stateChanged, this, &MusicManager::onStateChanged);
    audioManager->deleteLater();
  }
  audioManager = new QMediaPlayer(this);
  audioManager->setMedia(QUrl::fromLocalFile(ASSETS_PATH + "audio/" + currentTrack));
#ifndef _WIN32
  audioManager->play();
#endif
  setVolumeToDefault();
  connect(audioManager, &QMediaPlayer::stateChanged, this, &MusicManager::onStateChanged);
}

void MusicManager::onStateChanged(QMediaPlayer::State state)
{
  switch (state)
  {
  case QMediaPlayer::StoppedState:
  case QMediaPlayer::PausedState:
    playNext();
    break ;
  case QMediaPlayer::PlayingState:
    break ;
  }
}

void MusicManager::fadeVolume()
{
  int volume = audioManager->volume();

  if (volume > volumeGoal)
  {
    volume -= 5;
    if (volume < volumeGoal)
      volume = volumeGoal;
  }
  else
  {
    volume += 5;
    if (volume > volumeGoal)
      volume = volumeGoal;
  }
  audioManager->setVolume(volume);
  if (volume == volumeGoal)
  {
    fadingTimer.stop();
    if (nextTrack != "")
      fadeOut();
  }
}

void MusicManager::fadeOut()
{
  int volume = audioManager->volume();

  volumeGoal = 0;
  if (volume == volumeGoal)
  {
    audioManager->stop();
    startTrack(nextTrack);
    fadingOut    = false;
  }
  else
    fadingTimer.start();
}

void MusicManager::setVolume(int volume)
{
  volumeGoal = volume;
  audioManager->setVolume(volume);
}

void MusicManager::setVolumeToDefault()
{
  setVolume(getDefaultVolume());
}

void MusicManager::setDefaultVolume(int value)
{
  QSettings settings;

  settings.setValue("audio/volume", value);
  emit defaultVolumeChanged();
}

int MusicManager::getDefaultVolume() const
{
  return QSettings().value("audio/volume", 100).toInt();
}
