#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QVector>
#include <QSharedPointer>
#include <QTimer>
#include <QThread>

class QSoundEffect;
class DynamicObject;

class SoundWorker : public QObject
{
  Q_OBJECT
public:
  SoundWorker();
  ~SoundWorker();

public slots:
  void playSound(const QUrl& url, float volume);
private slots:
  void update();
private:
  QVector<QSharedPointer<QSoundEffect>> sounds;
  QTimer timer;
};

class SoundManager : public QObject
{
  static SoundManager* _global_ptr;

  Q_OBJECT

  Q_PROPERTY(int defaultVolume READ getDefaultVolume WRITE setDefaultVolume NOTIFY defaultVolumeChanged)
public:
  explicit SoundManager(QObject *parent = nullptr);
  ~SoundManager();

  static SoundManager* get(void) { return _global_ptr; }

  void initialize();
  void start();
  void stop();

  void setDefaultVolume(int value);
  int getDefaultVolume() const;

  Q_INVOKABLE void play(const QString&, qreal volume = 1.f);
  Q_INVOKABLE void play(const DynamicObject*, const QString&, qreal volume = 1.f);

signals:
  void defaultVolumeChanged();
  void playSound(const QUrl&, float volume);

private:
  bool initialized = false;
  QMap<QString, QUrl> soundLibrary;
  QThread* workerThread = nullptr;
};

#endif // SOUNDMANAGER_H
