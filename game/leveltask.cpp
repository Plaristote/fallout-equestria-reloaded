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
  registerDynamicObject(player);
  player->setSpriteName("pony");
  player->setAnimation("walking-down");
  forceCharacterPosition(player, 0, 0);
  //triggerCharacterMoveTo(player, 8, 2);
  if (grid->findPath(QPoint(0, 0), QPoint(10, 8), player->rcurrentPath()))
  {
    QPoint nextCase = player->getCurrentPath().first();
    qDebug() << "Ztarting our great journey";
    triggerCharacterMoveTo(player, nextCase.x(), nextCase.y());
  }
  else
    qDebug() << "NO PATH TOWARD (10,8)";
}

void LevelTask::tileClicked(int x, int y)
{
  QPoint position = player->getPosition();

  qDebug() << "Going to" << x << y;
  if (grid->findPath(position, QPoint(x, y), player->rcurrentPath()))
  {
    QPoint nextCase = player->getCurrentPath().first();
    qDebug() << "Ztarting our great journey" << x << y;
    triggerCharacterMoveTo(player, nextCase.x(), nextCase.y());
  }
  else
    qDebug() << "NO PATH TOWARD" << x << y;
}

void LevelTask::registerDynamicObject(DynamicObject* object)
{
  connect(object, &Sprite::movementFinished, this, &LevelTask::onObjectMovementFinished);
}

void LevelTask::unregisterDynamicObject(DynamicObject* object)
{
  disconnect(object, &Sprite::movementFinished, this, &LevelTask::onObjectMovementFinished);
}

void LevelTask::onObjectMovementFinished(Sprite* sprite)
{
  auto* object = reinterpret_cast<DynamicObject*>(sprite);

  if (object->rcurrentPath().size() > 0)
  {
    object->rcurrentPath().pop_front();
    qDebug() << "Object movement zinizhed: left = " << object->getCurrentPath().size();
    if (object->getCurrentPath().size() > 0)
    {
      QPoint nextCase = object->getCurrentPath().first();

      if (!triggerCharacterMoveTo(object, nextCase.x(), nextCase.y()))
      {
        qDebug() << "Path blocked" << nextCase;
        emit object->pathBlocked();
      }
    }
    else
    {
      qDebug() << "-> Reached deztination";
      object->setAnimation("idle-down");
      emit object->reachedDestination();
    }
  }
  else
    qDebug() << "!!";
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

bool LevelTask::triggerCharacterMoveTo(DynamicObject* character, int x, int y)
{
  QPoint renderPosition = tilemap->getLayer("ground")->getTile(x, y)->getRenderPosition();

  if (grid->moveObject(character, x, y))
  {
    character->moveTo(x, y, renderPosition);
    return true;
  }
  else
    qDebug() << "Cannot move to coordinate" << x << y;
  return false;
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
