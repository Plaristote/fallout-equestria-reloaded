#include "leveltask.h"
#include "game.h"
#include "characterdialog.h"
#include <QJsonArray>
#include "inventoryitem.h"

LevelTask::LevelTask(QObject *parent) : CombatComponent(parent)
{
  tilemap = new TileMap(this);
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
  /*
  CharacterParty* otherParty = new CharacterParty(this);
  Character* otherChar = new Character;
  otherChar->setStatistics(Game::get()->getDataEngine()->makeStatModel("tintin", "toto"));
  otherChar->setSpriteName("pony-green");
  otherChar->setAnimation("idle-down");
  otherChar->setScript("dummy.mjs");
  setCharacterPosition(otherChar, 8, 0);
  otherParty->addCharacter(otherChar);

  insertPartyIntoZone(Game::get()->getPlayerParty());
  insertPartyIntoZone(otherParty);
  */
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
    else if (type == "storage")
    {
      StorageObject* storageObject = new StorageObject(this);

      object = storageObject;
      // LOOTING TEsT
      InventoryItem* toto = new InventoryItem(this);
      toto->setObjectName("testItem");
      storageObject->getInventory()->addItem(toto);
    }
    else
    {
      object = new DynamicObject(this);
    }
    object->setObjectName(objectData["name"].toString());
    grid->moveObject(object, drawAt.x(), drawAt.y());
    object->setPosition(drawAt);
    object->setRenderPosition(renderPosition);
    object->setInteractionPosition(interactionPosition);
    object->setSpriteAnimation(customDisplay);
    if (!scriptName.isEmpty())
      object->setScript(scriptName);
    registerDynamicObject(object);
  }
  //moveTo(otherChar, 0, 0);
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
    else if (type == "StorageObject")
      object = new StorageObject(this);
    else
      object = new DynamicObject(this);
    object->load(objectData.toObject());
    grid->moveObject(object, object->getPosition().x(), object->getPosition().y());
    object->setRenderPosition(object->getSpritePosition()); // isn't this basically self-assign ?
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

  if (getPlayer())
  {
    // Cancel interaction if any is running
    emit interactionRequired(nullptr, QStringList());
    // Infer action type and proceed
    if (occupant && openInteractionMenu(occupant))
      return ;
    else if (!moveTo(getPlayer(), x, y))
      emit displayConsoleMessage("No path towards [" + QString::number(x) + ',' + QString::number(y) + ']');
  }
  else
  {
    emit clickedOnCase(x, y);
    emit clickedOnObject(occupant);
  }
}

void LevelTask::registerDynamicObject(DynamicObject* object)
{
  objects.push_back(object);
  CombatComponent::registerDynamicObject(object);
  connect(object, &DynamicObject::controlZoneAdded,   this, &LevelTask::registerControlZone);
  connect(object, &DynamicObject::controlZoneRemoved, this, &LevelTask::unregisterControlZone);
  if (object->getControlZone())
    registerControlZone(object->getControlZone());
  emit objectsChanged();
}

void LevelTask::unregisterDynamicObject(DynamicObject* object)
{
  objects.removeAll(object);
  CombatComponent::unregisterDynamicObject(object);
  disconnect(object, &DynamicObject::controlZoneAdded,   this, &LevelTask::registerControlZone);
  disconnect(object, &DynamicObject::controlZoneRemoved, this, &LevelTask::unregisterControlZone);
  if (object->getControlZone())
    unregisterControlZone(object->getControlZone());
  emit objectsChanged();
}

void LevelTask::registerControlZone(TileZone* zone)
{
  tilemap->addTileZone(zone);
}

void LevelTask::unregisterControlZone(TileZone* zone)
{
  tilemap->removeTileZone(zone);
}

DynamicObject* LevelTask::getObjectByName(const QString& name)
{
  for (DynamicObject* object : objects)
  {
    if (object->getObjectName() == name)
      return object;
  }
  return nullptr;
}

QPoint LevelTask::getRenderPositionForTile(int x, int y)
{
  auto* layer = tilemap->getLayer("ground");
  auto* tile  = layer ? layer->getTile(x, y) : nullptr;

  return tile ? tile->getRenderPosition() : QPoint();
}

void LevelTask::onPauseChanged()
{
  qDebug() << "LevelTask::onPauseChanged:" << paused;
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
  CombatComponent::update(delta);
  emit updated();
}

Character* LevelTask::generateCharacter(const QString &name, const QString &characterSheet)
{
  Character* object = new Character(this);
  object->setObjectName(name);
  object->setStatistics(Game::get()->getDataEngine()->makeStatModel(characterSheet));
  registerDynamicObject(object);
  return object;
}

StorageObject* LevelTask::generateStorageObject(const QString &name)
{
  StorageObject* object = new StorageObject(this);

  object->setObjectName(name);
  registerDynamicObject(object);
  return object;
}
