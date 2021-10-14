#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QVector>
#include <QSharedPointer>

class QSoundEffect;
class DynamicObject;

class SoundManager : public QObject
{
  Q_OBJECT

public:
  explicit SoundManager(QObject *parent = nullptr);

  void update();

  Q_INVOKABLE void play(const QString&, qreal volume = 1.f);
  Q_INVOKABLE void play(const DynamicObject*, const QString&, qreal volume = 1.f);

private:
  QMap<QString, QUrl> soundLibrary;
  QVector<QSharedPointer<QSoundEffect>> sounds;
};

#endif // SOUNDMANAGER_H
