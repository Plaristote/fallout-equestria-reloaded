#include "leveltask.h"
#include "game.h"
#include <QJsonArray>

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

  for (QJsonObject objectData : tilemap->getObjects())
  {
    DynamicObject*  object;
    QJsonArray      objectTiles = objectData["objects"].toArray();
    QJsonObject     objectTile;
    int             gid;
    QString         type;
    QPoint          drawAt, interactionPosition;
    QString         scriptName, dialogName;
    QPoint          renderPosition;
    SpriteAnimation customDisplay;

    qDebug() << ">>>>>>>>>>>>>>>>>>>>> Loading object" << objectData["name"].toString();
    if (objectTiles.size() == 0)
      continue ;
    objectTile  = objectTiles.first().toObject();
    gid         = objectTile["gid"].toInt();
    customDisplay.name   = "tiled-object";
    customDisplay.source = tilemap->getObjectSource(gid);
    customDisplay.clippedRect.setSize(tilemap->getObjectSize(gid));
    customDisplay.frameCount = 1;
    renderPosition.setX(static_cast<int>(objectTile["x"].toDouble())); // unreliable, use properties x/y
    renderPosition.setY(static_cast<int>(objectTile["y"].toDouble()));
    for (QJsonValue property : objectData["properties"].toArray())
    {
        QString propertyName = property["name"].toString();
        QJsonValue propertyValue = property["value"];

        if (propertyName == "type")
          type = propertyValue.toString();
        else if (propertyName == "drawAtX")
          drawAt.setX(propertyValue.toInt());
        else if (propertyName == "drawAtY")
          drawAt.setY(propertyValue.toInt());
        else if (propertyName == "interactionPositionX")
          interactionPosition.setX(propertyValue.toInt());
        else if (propertyName == "interactionPositionY")
          interactionPosition.setY(propertyValue.toInt());
        else if (propertyName == "script")
          scriptName = propertyValue.toString();
        else if (propertyName == "dialog")
          dialogName = propertyValue.toString();

        else if (propertyName == "x")
          renderPosition.setX(propertyValue.toInt() - this->tilemap->getPixelWidth() / 2 + customDisplay.clippedRect.width() / 2 + 5);
        else if (propertyName == "y")
          renderPosition.setY(propertyValue.toInt());
    }
    if (type == "character")
    {
      Character* character = new Character(this);

      object = character;
    }
    else
    {
      object = new DynamicObject(this);
    }
    grid->moveObject(object, drawAt.x(), drawAt.y());
    object->forceMoveToCoordinates(renderPosition);
    object->setInteractionPosition(interactionPosition);
    object->setSpriteAnimation(customDisplay);
    if (!scriptName.isEmpty())
      object->setScript(scriptName);
    registerDynamicObject(object);
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
  DynamicObject* occupant = grid->getOccupant(x, y);

  // Cancel interaction if any is running
  emit interactionRequired(nullptr, QStringList());
  // Infer interaction type and proceed
  if (occupant && openInteractionMenu(occupant))
    return ;
  else if (!moveCharacterTo(player, x, y))
    emit displayConsoleMessage("No path towards [" + QString::number(x) + ',' + QString::number(y) + ']');
}

bool LevelTask::openInteractionMenu(DynamicObject* object)
{
  auto entries = object->getAvailableInteractions();

  if (entries.length() > 0)
  {
    qDebug() << "interaction rekired called";
    emit interactionRequired(object, entries);
    return true;
  }
  else
    qDebug() << "no interction available for object";
  return false;
}

void LevelTask::interactOrderReceived(DynamicObject* object, const QString &type)
{
  auto position = object->getInteractionPosition();

  pendingInteraction.first  = object;
  pendingInteraction.second = type;
  if (player->getPosition() != position)
  {
    if (position.x() != -1 && !moveCharacterTo(player, position.x(), position.y()))
    {
      pendingInteraction.first = nullptr;
      displayConsoleMessage("Cannot reach target.");
    }
    else
      qDebug() << "Going to target at " << position;
  }
  else
    startPendingInteraction();
}

void LevelTask::startPendingInteraction()
{
  if (pendingInteraction.first)
  {
    if (pendingInteraction.second == "talk-to")
    {
      displayConsoleMessage("Should try to start interaction " + pendingInteraction.second);
      emit startDialog(reinterpret_cast<Character*>(pendingInteraction.first));
    }
    else
      qDebug() << "Error 422: unknown interaciton type";
  }
  else
    qDebug() << "Error 500: No more interaction target";
  pendingInteraction.first = nullptr;
}

void LevelTask::registerDynamicObject(DynamicObject* object)
{
  objects.push_back(object);
  connect(object, &Sprite::movementFinished, this, &LevelTask::onObjectMovementFinished);
}

void LevelTask::unregisterDynamicObject(DynamicObject* object)
{
  objects.removeAll(object);
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
        if (sprite == player) { pendingInteraction.first = nullptr; }
        emit object->pathBlocked();
      }
    }
    else
    {
      qDebug() << "-> Reached deztination";
      object->setAnimation("idle-down");
      if (sprite == player && pendingInteraction.first != nullptr) { startPendingInteraction(); }
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
