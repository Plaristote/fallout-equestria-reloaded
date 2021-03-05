#include "musicmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

using namespace std;

MusicManager* MusicManager::_global_ptr = nullptr;

MusicManager::MusicManager(QObject* parent) : QObject(parent)
{
  audioManager = new QMediaPlayer(this);
  _global_ptr  = this;
  fadingOut    = false;
  volumeRef    = 100;
  volumeGoal   = volumeRef;
  loadDataTree();
  setVolume(100);
  fadingTimer.setInterval(50);
  fadingTimer.setSingleShot(false);
  // TODO implements OptionsManager
  //connect(OptionsManager::get(), &OptionsManager::updated, this, &MusicManager::setVolumeToDefault);
  connect(&fadingTimer,  &QTimer::timeout,            this, &MusicManager::fadeVolume);
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

void MusicManager::playNext(void)
{
  const QJsonObject    category = dataTree[currentCategory].toObject();
  const QStringList    keys     = category.keys();
  const unsigned short max      = static_cast<unsigned short>(keys.length());

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
  if (filename == "")
    throw "quoi";
  currentTrack = filename;
  nextTrack    = currentTrack;
  delete audioManager;
  audioManager = new QMediaPlayer(this);
  audioManager->setMedia(QUrl::fromLocalFile(ASSETS_PATH + "audio/" + currentTrack));
  audioManager->play();
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
    setVolumeToDefault();
    volumeGoal   = volumeRef;
    fadingOut    = false;
  }
  else
    fadingTimer.start();
}

void MusicManager::setVolume(int volume)
{
  volumeRef  = volume;
  volumeGoal = volume;
}

void MusicManager::setVolumeToDefault()
{
  // TODO implement OptionsManager and use it to set default volume
  setVolume(100);
  //float volume = OptionsManager::Get()["music"]["volume"].Or(5.f);

  //SetVolume(volume);
}
