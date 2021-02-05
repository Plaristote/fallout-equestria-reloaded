#include "leveltask.h"

LevelTask::LevelTask(QObject *parent) : QObject(parent)
{
  tilemap = new TileMap(this);
  taskTick.setInterval(17);
  clockTick.setInterval(1000);
  connect(&taskTick,  &QTimer::timeout, this, &LevelTask::onTaskTick);
  connect(&clockTick, &QTimer::timeout, this, &LevelTask::onClockTick);
}

void LevelTask::load(const QString& levelName)
{
  tilemap->load(levelName);
}

void LevelTask::onTaskTick()
{

}

void LevelTask::onClockTick()
{

}

void LevelTask::moveTo(int x, int y)
{

}

void LevelTask::onPauseChanged()
{
  if (paused)
  {
    taskTick.stop();
    clockTick.stop();
  }
  else
  {
    taskTick.start();
    clockTick.start();
  }
}
