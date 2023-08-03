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

  Q_PROPERTY(int defaultVolume READ getDefaultVolume WRITE setDefaultVolume NOTIFY defaultVolumeChanged)

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  typedef QMediaPlayer::State PlaybackState;
#else
  typedef QMediaPlayer::PlaybackState PlaybackState;
#endif

public:
  explicit MusicManager(QObject* parent = nullptr);
  ~MusicManager();

  static MusicManager* get(void)          { return (_global_ptr); }

  Q_INVOKABLE void     play(const QString& category);
  Q_INVOKABLE void     play(const QString& category, const QString& name);
  Q_INVOKABLE void     pause(bool paused);
  Q_INVOKABLE void     playNext(void);
  Q_INVOKABLE void     setVolume(int volume);
  Q_INVOKABLE void     setVolumeToDefault(void);
  void                 setDefaultVolume(int);
  int                  getDefaultVolume() const;

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

  QJsonObject          data;
  QJsonObject          dataTree;
  QString              currentCategory;
  QTimer               fadingTimer;
  bool                 fadingOut;
  int                  volumeGoal;
  int                  volumeRef;
  QMediaPlayer*        audioManager = nullptr;
  QString              currentTrack, nextTrack;
};

#endif
