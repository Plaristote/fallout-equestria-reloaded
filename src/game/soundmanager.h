#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QVector>
#include <QSharedPointer>
#include <QTimer>

class QSoundEffect;
class DynamicObject;

class SoundManager : public QObject
{
  static SoundManager* _global_ptr;

  Q_OBJECT

  Q_PROPERTY(int defaultVolume READ getDefaultVolume WRITE setDefaultVolume NOTIFY defaultVolumeChanged)
public:
  explicit SoundManager(QObject *parent = nullptr);

  static SoundManager* get(void) { return _global_ptr; }

  void initialize();
  void stop();

  void setDefaultVolume(int value);
  int getDefaultVolume() const;

  Q_INVOKABLE void play(const QString&, qreal volume = 1.f);
  Q_INVOKABLE void play(const DynamicObject*, const QString&, qreal volume = 1.f);

signals:
  void defaultVolumeChanged();

private slots:
  void update();

private:
  bool initialized = false;
  QMap<QString, QUrl> soundLibrary;
  QVector<QSharedPointer<QSoundEffect>> sounds;
  QTimer timer;
};

#endif // SOUNDMANAGER_H
