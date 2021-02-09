#include "leveltask.h"
#include "game.h"

LevelTask::LevelTask(QObject *parent) : QObject(parent)
{
  player  = nullptr;
  tilemap = new TileMap(this);
  grid    = new LevelGrid(this);
  taskTick.setInterval(17);
  clockTick.setInterval(1000);
  connect(&taskTick,  &QTimer::timeout, this, &LevelTask::onTaskTick);
  connect(&clockTick, &QTimer::timeout, this, &LevelTask::onClockTick);
}

void LevelTask::load(const QString& levelName)
{
  name = levelName;
  tilemap->load(levelName);
  grid->initializeGrid(tilemap);

  CharacterParty* otherParty = new CharacterParty(this);
  Character* otherChar = new Character;
  otherChar->setSpriteName("pony-green");
  otherChar->setAnimation("idle-down");
  otherChar->setScript("dummy.mjs");
  otherParty->addCharacter(otherChar);

  for (auto* zone : tilemap->getZones())
  {
    connect(zone, &TileZone::enteredZone, this, &LevelTask::onZoneEntered, Qt::QueuedConnection);
    connect(zone, &TileZone::exitedZone,  this, &LevelTask::onZoneExited,  Qt::QueuedConnection);
    if (zone->getType() == "entry" && zone->getIsDefault())
    {
      auto* playerParty = Game::get()->getPlayerParty();

      player = playerParty->getCharacters().first();
      playerParty->insertIntoZone(this, zone);
      //
      otherParty->insertIntoZone(this, zone);
    }
  }
  if (player == nullptr)
    qDebug()<< "Could not input player !";

  moveCharacterTo(otherChar, 0, 0);
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
      {
        displayConsoleMessage("TODO: go to " + zone->getTarget());
        emit exitZoneEntered(zone);
      }
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
  if (!moveCharacterTo(player, x, y))
    emit displayConsoleMessage("No path towards [" + QString::number(x) + ',' + QString::number(y) + ']');
}

void LevelTask::registerDynamicObject(DynamicObject* object)
{
  connect(object, &Sprite::movementFinished, this, &LevelTask::onObjectMovementFinished);
}

void LevelTask::unregisterDynamicObject(DynamicObject* object)
{
  object->setAnimation("idle-down");
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

bool LevelTask::moveCharacterTo(DynamicObject* character, int x, int y)
{
  // TODO Pathfinding and all that shit
  QPoint position = player->getPosition();

  if (grid->findPath(position, QPoint(x, y), character->rcurrentPath()))
  {
    if (character->getCurrentPath().size() > 0)
    {
      QPoint nextCase = character->getCurrentPath().first();
      triggerCharacterMoveTo(character, nextCase.x(), nextCase.y());
    }
    else
      emit character->reachedDestination();
    return true;
  }
  return false;

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
