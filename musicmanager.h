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

public:
  explicit MusicManager(QObject* parent = nullptr);
  ~MusicManager();

  static MusicManager* get(void)          { return (_global_ptr); }

  Q_INVOKABLE void     play(const QString& category);
  Q_INVOKABLE void     play(const QString& category, const QString& name);
  Q_INVOKABLE void     pause(bool paused);
  void                 playNext(void);
  Q_INVOKABLE void     setVolume(int volume);
  Q_INVOKABLE void     setVolumeToDefault(void);

private slots:
  void                 fadeOut();
  void                 fadeVolume();
  void                 onStateChanged(QMediaPlayer::State);

private:
  void                 loadDataTree();
  void                 startTrack(const QString& filename);

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
