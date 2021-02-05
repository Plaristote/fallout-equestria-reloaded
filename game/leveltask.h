#ifndef  LEVELTASK_H
# define LEVELTASK_H

# include <QObject>
# include <QTimer>
# include "tilemap/tilemap.h"

class LevelTask : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool     paused MEMBER paused NOTIFY pausedChanged)
  Q_PROPERTY(TileMap* tilemap READ getTileMap NOTIFY tilemapReady)
public:
  explicit LevelTask(QObject *parent = nullptr);

  void load(const QString& levelName);

  TileMap* getTileMap() const { return tilemap; }

  Q_INVOKABLE void moveTo(int x, int y);

signals:
  void pausedChanged();
  void tilemapReady();

private slots:
  void onPauseChanged();
  void onClockTick();
  void onTaskTick();

private:
  QTimer   taskTick, clockTick;
  TileMap* tilemap = nullptr;
  bool     paused = true;
};

#endif // LEVELTASK_H
