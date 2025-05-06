#ifndef  MUSICMANAGER_H
# define MUSICMANAGER_H

# include "globals.h"
# include <string>
# include <QTimer>
# include <QJsonObject>
# include <QMediaPlayer>

class MusicManager : public QObject
{
  static MusicManager* _global_ptr;

  Q_OBJECT
  Q_PROPERTY(double defaultVolume READ getDefaultVolume WRITE setDefaultVolume NOTIFY defaultVolumeChanged)

  typedef QMediaPlayer::PlaybackState PlaybackState;
public:
  explicit MusicManager(QObject* parent = nullptr);
  ~MusicManager();

  static MusicManager* get(void)          { return (_global_ptr); }

  Q_INVOKABLE void     play(const QString& category);
  Q_INVOKABLE void     play(const QString& category, const QString& name);
  Q_INVOKABLE void     pause(bool paused);
  Q_INVOKABLE void     playNext(void);
  Q_INVOKABLE void     setVolume(double volume);
  Q_INVOKABLE void     setVolumeToDefault(void);
  void                 setDefaultVolume(double);
  double               getDefaultVolume() const;

signals:
  void defaultVolumeChanged();

private slots:
  void                 fadeOut();
  void                 fadeVolume();
  void                 onStateChanged(PlaybackState);

private:
  void                 loadDataTree();
  void                 startTrack(const QString& filename);
  QStringList          potentialSuccessors() const;
  void                 cleanupAudioManager();

  QJsonObject          data;
  QJsonObject          dataTree;
  QString              currentCategory;
  QTimer               fadingTimer;
  bool                 fadingOut;
  double               volumeGoal;
  QMediaPlayer*        audioManager = nullptr;
  QAudioOutput*        audioOutput = nullptr;
  QString              currentTrack, nextTrack;
};

#endif
