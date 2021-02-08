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
  for (auto* zone : tilemap->getZones())
  {
    connect(zone, &TileZone::enteredZone, this, &LevelTask::onZoneEntered, Qt::QueuedConnection);
    connect(zone, &TileZone::exitedZone,  this, &LevelTask::onZoneExited,  Qt::QueuedConnection);
  }

  player = new DynamicObject(this);
  registerDynamicObject(player);
  player->setSpriteName("pony");
  player->setAnimation("idle-down");
  forceCharacterPosition(player, 0, 0);
}

void LevelTask::onZoneEntered(DynamicObject* object, TileZone* zone)
{
  if (object == player)
  {
    displayConsoleMessage("Zone entered: " + zone->getName());
    if (zone->getType() == "exit")
    {
      if (zone->getTarget() == "")
        displayConsoleMessage("TODO: go to worldmap");
      else
        displayConsoleMessage("TODO: go to " + zone->getTarget());
    }
  }
}

void LevelTask::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (object == player)
  {
    displayConsoleMessage("Zone exited: " + zone->getName());
  }
}

void LevelTask::tileClicked(int x, int y)
{
  QPoint position = player->getPosition();

  if (grid->findPath(position, QPoint(x, y), player->rcurrentPath()))
  {
    if (player->getCurrentPath().size() > 0)
    {
      QPoint nextCase = player->getCurrentPath().first();
      triggerCharacterMoveTo(player, nextCase.x(), nextCase.y());
    }
    else
      emit player->reachedDestination();
  }
  else
    emit displayConsoleMessage("No path towards [" + QString::number(x) + ',' + QString::number(y) + ']');
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
  auto position = object->getPosition();

  grid->triggerZone(object, position.x(), position.y());
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
  character->setPosition(QPoint(x, y));
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
