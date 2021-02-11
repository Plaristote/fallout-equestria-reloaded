#include "leveltask.h"
#include "game.h"
#include "characterdialog.h"
#include <QJsonArray>

LevelTask::LevelTask(QObject *parent) : QObject(parent)
{
  tilemap = new TileMap(this);
  grid    = new LevelGrid(this);
  updateTimer.setInterval(30);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &LevelTask::update);
  connect(this, &LevelTask::pausedChanged, this, &LevelTask::onPauseChanged);
}

Character* LevelTask::getPlayer()
{
  return Game::get()->getPlayer();
}

bool LevelTask::insertPartyIntoZone(CharacterParty* party, const QString& zoneName)
{
  for (auto* zone : tilemap->getZones())
  {
    if (zone->getType() == "entry" && (zone->getName() == zoneName || (zone->getIsDefault() && zoneName == "")))
      return party->insertIntoZone(this, zone);
  }
  return false;
}

void LevelTask::load(const QString& levelName, DataEngine* dataEngine)
{
  name = levelName;
  timeManager = Game::get()->getTimeManager();
  tilemap->load(levelName);
  grid->initializeGrid(tilemap);

  if (dataEngine->isLevelActive(name))
    loadObjectsFromDataEngine(dataEngine);
  else
    loadObjectsFromTilemap();
  for (auto* zone : tilemap->getZones())
  {
    connect(zone, &TileZone::enteredZone, this, &LevelTask::onZoneEntered, Qt::QueuedConnection);
    connect(zone, &TileZone::exitedZone,  this, &LevelTask::onZoneExited,  Qt::QueuedConnection);
  }
}

void LevelTask::loadObjectsFromTilemap()
{
  displayConsoleMessage("Level loaded from TileMap");

  // TODO thatz tezt data
  CharacterParty* otherParty = new CharacterParty(this);
  Character* otherChar = new Character;
  otherChar->setStatistics(Game::get()->getDataEngine()->makeStatModel("tintin", "toto"));
  otherChar->setSpriteName("pony-green");
  otherChar->setAnimation("idle-down");
  otherChar->setScript("dummy.mjs");
  forceCharacterPosition(otherChar, 8, 0);
  otherParty->addCharacter(otherChar);

  insertPartyIntoZone(Game::get()->getPlayerParty());
  insertPartyIntoZone(otherParty);
  // END tezt data

  for (QJsonObject objectData : tilemap->getObjects())
  {
    DynamicObject*  object;
    QJsonArray      objectTiles = objectData["objects"].toArray();
    QJsonObject     objectTile;
    int             gid;
    QString         type;
    QPoint          drawAt, interactionPosition;
    QString         scriptName, dialogName, characterSheetName;
    QPoint          renderPosition;
    SpriteAnimation customDisplay;
    bool            unique = false;

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
        else if (propertyName == "characterSheet")
          characterSheetName = propertyValue.toString();
        else if (propertyName == "unique")
          characterSheetName = propertyValue.toBool();

        else if (propertyName == "x")
          renderPosition.setX(propertyValue.toInt() - this->tilemap->getPixelWidth() / 2 + customDisplay.clippedRect.width() / 2 + 5);
        else if (propertyName == "y")
          renderPosition.setY(propertyValue.toInt());
    }
    if (type == "character")
    {
      Character* character = new Character(this);

      character->setUnique(unique);
      character->setStatistics(
        Game::get()->getDataEngine()->makeStatModel(character->getObjectName(), characterSheetName)
      );
      object = character;
    }
    else
    {
      object = new DynamicObject(this);
    }
    object->setObjectName(objectData["name"].toString());
    grid->moveObject(object, drawAt.x(), drawAt.y());
    object->setPosition(drawAt);
    object->forceMoveToCoordinates(renderPosition);
    object->setInteractionPosition(interactionPosition);
    object->setSpriteAnimation(customDisplay);
    if (!scriptName.isEmpty())
      object->setScript(scriptName);
    registerDynamicObject(object);
  }
  moveCharacterTo(otherChar, 0, 0);
}

void LevelTask::loadObjectsFromDataEngine(DataEngine* dataEngine)
{
  QJsonObject levelData = dataEngine->getLevelData(name);

  displayConsoleMessage("Level loaded from DataEngine");

  for (QJsonValue objectData : levelData["objects"].toArray())
  {
    QString type = objectData["type"].toString();
    DynamicObject* object;

    if (type == "Character")
      object = new Character(this);
    else
      object = new DynamicObject(this);
    object->load(objectData.toObject());
    grid->moveObject(object, object->getPosition().x(), object->getPosition().y());
    //object->forceMoveToCoordinates(object->getSpritePosition());
    registerDynamicObject(object);

    if (type == "Character") {
      // TODO Execute pending saved actions
    }
  }
}

void LevelTask::save(DataEngine* dataEngine)
{
  QJsonObject levelData = dataEngine->getLevelData(name);
  QJsonArray  objectArray;
  auto*       playerParty = Game::get()->getPlayerParty();

  for (DynamicObject* object : objects)
  {
    QJsonObject objectData;
    QString     type(object->metaObject()->className());

    if (type != "Character" || !playerParty->containsCharacter(reinterpret_cast<Character*>(object)))
    {
      objectData["type"] = type;
      object->save(objectData);
      objectArray << objectData;
    }
  }
  levelData["objects"] = objectArray;
  dataEngine->setLevelData(name, levelData);
}

void LevelTask::onZoneEntered(DynamicObject* object, TileZone* zone)
{
  if (object == getPlayer())
  {
    displayConsoleMessage("Zone entered: " + zone->getName());
    if (zone->getType() == "exit")
      emit exitZoneEntered(zone);
  }
}

void LevelTask::onZoneExited(DynamicObject* object, TileZone* zone)
{
  if (object == getPlayer())
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
  else if (!moveCharacterTo(getPlayer(), x, y))
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
  auto  position = object->getInteractionPosition();
  auto* player = getPlayer();

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
      Character*       npc    = reinterpret_cast<Character*>(pendingInteraction.first);
      CharacterDialog* dialog = new CharacterDialog(this);

      dialog->load(npc->getDialogName(), getPlayer(), npc);
      displayConsoleMessage("Should try to start interaction " + pendingInteraction.second);
      emit startDialog(dialog);
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
        if (sprite == getPlayer()) { pendingInteraction.first = nullptr; }
        emit object->pathBlocked();
      }
    }
    else
    {
      qDebug() << "-> Reached deztination";
      object->setAnimation("idle-down");
      if (sprite == getPlayer() && pendingInteraction.first != nullptr) { startPendingInteraction(); }
      emit object->reachedDestination();
    }
  }
  else
    qDebug() << "!!";
}

DynamicObject* LevelTask::getOccupantAt(int x, int y)
{
  return grid->getOccupant(x, y);
}

void LevelTask::moveTo(int x, int y)
{
  moveCharacterTo(getPlayer(), x, y);
}

bool LevelTask::moveCharacterTo(DynamicObject* character, int x, int y)
{
  QPoint position = getPlayer()->getPosition();

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
  qDebug() << "PAUZE CHANGED";
  if (paused)
    updateTimer.stop();
  else
  {
    updateTimer.start();
    clock.start();
  }
}

void LevelTask::update()
{
  qint64 delta = clock.restart();

  timeManager->addElapsedMilliseconds(delta);
  for (DynamicObject* object : objects)
    object->update(delta);
  emit updated();
}
