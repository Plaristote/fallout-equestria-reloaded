#include "leveltask.h"

LevelTask::LevelTask(QObject *parent) : QObject(parent)
{
  tilemap = new TileMap(this);
  grid    = new LevelGrid(this);
  taskTick.setInterval(17);
  clockTick.setInterval(1000);
  connect(&taskTick,  &QTimer::timeout, this, &LevelTask::onTaskTick);
  connect(&clockTick, &QTimer::timeout, this, &LevelTask::onClockTick);
}

void LevelTask::load(const QString& levelName)
{
  tilemap->load(levelName);
  grid->initializeGrid(tilemap);

  player = new DynamicObject(this);
  player->setSpriteName("pony");
  player->setAnimation("walking-down");
  forceCharacterPosition(player, 0, 0);
  triggerCharacterMoveTo(player, 8, 2);
}

void LevelTask::onTaskTick()
{

}

void LevelTask::onClockTick()
{

}

DynamicObject* LevelTask::getOccupantAt(int x, int y)
{
  return grid->getOccupant(x, y);
}

void LevelTask::moveTo(int x, int y)
{
  moveCharacterTo(player, x, y);
}

void LevelTask::moveCharacterTo(DynamicObject *, int x, int y)
{
  // TODO Pathfinding and all that shit
}

void LevelTask::triggerCharacterMoveTo(DynamicObject* character, int x, int y)
{
  QPoint renderPosition = tilemap->getLayer("ground")->getTile(x, y)->getRenderPosition();

  if (grid->moveObject(character, x, y))
    character->moveToCoordinates(renderPosition);
  else
    qDebug() << "Cannot move to coordinate" << x << y;
}

void LevelTask::forceCharacterPosition(DynamicObject* character, int x, int y)
{
  QPoint renderPosition = tilemap->getLayer("ground")->getTile(x, y)->getRenderPosition();

  grid->moveObject(character, x, y);
  character->forceMoveToCoordinates(renderPosition);
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
