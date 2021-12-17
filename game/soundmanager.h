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
  Q_OBJECT

public:
  explicit SoundManager(QObject *parent = nullptr);

  Q_INVOKABLE void play(const QString&, qreal volume = 1.f);
  Q_INVOKABLE void play(const DynamicObject*, const QString&, qreal volume = 1.f);

private slots:
  void update();

private:
  QMap<QString, QUrl> soundLibrary;
  QVector<QSharedPointer<QSoundEffect>> sounds;
  QTimer timer;
};

#endif // SOUNDMANAGER_H
