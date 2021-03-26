#include "leveltask.h"
#include "game.h"
#include "characterdialog.h"
#include <QJsonArray>
#include "characters/actionqueue.h"
#include "objects/inventoryitem.h"
#include "objects/doorway.h"

LevelTask::LevelTask(QObject *parent) : CombatComponent(parent)
{
  tilemap = new TileMap(this);
  soundManager = new SoundManager(this);
  taskRunner = new TaskRunner(this);
  updateTimer.setInterval(15);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &LevelTask::update);
  connect(this, &LevelTask::pausedChanged, this, &LevelTask::onPauseChanged);
  connect(this, &LevelTask::combatChanged, this, &LevelTask::onCombatChanged);
}

LevelTask::~LevelTask()
{
  qDebug() << "LevelTask::destroyed";
  if (script)
    delete script;
}

void LevelTask::deleteLater()
{
  updateTimer.stop();
  CombatComponent::deleteLater();
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
  loadTilemap(levelName);
  grid->initializeGrid(tilemap);
  script = new ScriptController(SCRIPTS_PATH + "levels/" + levelName + ".mjs");
  script->initialize(this);
  taskRunner->setScriptController(script);
  for (auto* zone : tilemap->getZones())
    registerZone(zone);
  if (dataEngine->isLevelActive(name))
    loadObjectsFromDataEngine(dataEngine);
  if (Game::get()->property("isGameEditor").toBool())
    connect(this, &LevelTask::objectsChanged, this, &LevelTask::visibleCharactersChanged);
  else
    connect(getPlayer()->getFieldOfView(), &FieldOfView::refreshed, this, &LevelTask::visibleCharactersChanged);
}

void LevelTask::loadTilemap(const QString& levelName)
{
  tilemap->load(levelName);
  tilemap->getLayer("ground")->renderToFile("_tilemap.png");
  for (TileLayer* roofLayer : tilemap->getRoofs())
  {
    QString fileName = "_roof_" + roofLayer->getName() + ".png";

    roofLayer->renderToFile(fileName);
  }
  for (TileLayer* lightLayer : tilemap->getLights())
  {
    QString fileName = "_lights_" + lightLayer->getName() + ".png";

    lightLayer->renderToFile(fileName);
  }
  GridComponent::load();
  emit tilemapReady();
}

void LevelTask::loadObjectsFromDataEngine(DataEngine* dataEngine)
{
  QJsonObject levelData = dataEngine->getLevelData(name);
  QJsonValue  lastUpdate = levelData["lastUpdate"];

  dataStore = levelData["vars"].toObject();
  for (QJsonValue objectData : levelData["objects"].toArray())
  {
    QString type = objectData["type"].toString();
    DynamicObject* object;
    Character* character = nullptr;

    if (type == "Character")
    {
      character = new Character(this);
      object = character;
    }
    else if (type == "StorageObject")
      object = new StorageObject(this);
    else if (type == "Doorway")
      object = new Doorway(this);
    else if (type == "InventoryItem")
      object = new InventoryItem(this);
    else
      object = new DynamicObject(this);
    object->load(objectData.toObject());
    object->setRenderPosition(object->getSpritePosition()); // isn't this basically self-assign ?
    registerDynamicObject(object);
  }
  taskRunner->load(levelData["tasks"].toObject());
  if (!lastUpdate.isUndefined() && !lastUpdate.isNull())
  {
    std::time_t timestamp    = static_cast<std::time_t>(lastUpdate.toInt());
    std::time_t newTimestamp = Game::get()->getTimeManager()->getDateTime().GetTimestamp();
    std::time_t elapsedTime  = newTimestamp - timestamp;

    qDebug() << "ADVANCING TIME IN LEVEL" << timestamp << newTimestamp << elapsedTime;
    for (DynamicObject* object : qAsConst(objects))
      object->getTaskManager()->update(elapsedTime * 1000);
    taskRunner->update(elapsedTime *  1000);
  }
}

void LevelTask::save(DataEngine* dataEngine)
{
  Game* game = Game::get();
  QJsonObject levelData = dataEngine->getLevelData(name);
  QJsonObject taskData;
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
  if (!(game->property("isGameEditor").toBool()))
  {
    levelData["lastUpdate"] = static_cast<int>(game->getTimeManager()->getDateTime().GetTimestamp());
    levelData.insert("vars", dataStore);
    taskRunner->save(taskData);
    levelData.insert("tasks", taskData);
  }
  dataEngine->setLevelData(name, levelData);
}

void LevelTask::tileClicked(int x, int y)
{
  DynamicObject* occupant = grid->getOccupant(x, y);

  if (getPlayer())
  {
    if (occupant)
    {
      QPoint interactionPosition = occupant->getInteractionPosition();
      x = interactionPosition.x();
      y = interactionPosition.y();
    }
    getPlayer()->getActionQueue()->reset();
    getPlayer()->getActionQueue()->pushMovement(QPoint(x, y));
    if (!(getPlayer()->getActionQueue()->start()))
      displayConsoleMessage("No path.");
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
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    addCharacterObserver(character, connect(character, &Character::itemDropped, [this, character](InventoryItem* item) { onItemDropped(item, character->getPosition()); }));
    addCharacterObserver(character, connect(character, &Character::characterKill, this, &LevelTask::onCharacterKill));
  }
  emit objectsChanged();
}

void LevelTask::unregisterDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
  {
    auto* character = reinterpret_cast<Character*>(object);
    for (auto* entry : objects)
    {
      if (entry->isCharacter())
        reinterpret_cast<Character*>(entry)->getFieldOfView()->removeCharacter(character);
    }
  }
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
  registerZone(zone);
}

void LevelTask::unregisterControlZone(TileZone* zone)
{
  unregisterZone(zone);
}

void LevelTask::onItemDropped(InventoryItem* item, QPoint position)
{
  item->setParent(this);
  item->setPosition(position);
  registerDynamicObject(item);
  setObjectPosition(item, position.x(), position.y());
}

QList<Character*> LevelTask::findCharacters(std::function<bool (Character &)> compare)
{
  QList<Character*> characters;

  for (DynamicObject* object : objects)
  {
    if (object->isCharacter())
    {
      Character* character = reinterpret_cast<Character*>(object);

      if (compare(*character))
        characters << character;
    }
  }
  return characters;
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

void LevelTask::onCharacterKill(Character* victim, Character* killer)
{
  auto* playerParty = Game::get()->getPlayerParty();
  auto* victimSheet = victim->getStatistics();
  QString victimRace = victimSheet->property("race").toString();

  Game::get()->getQuestManager()->onCharacterKilled(victim, killer);
  if (killer)
    killer->getStatistics()->addKill(victimRace.length() > 0 ? victimRace : "Others");
  if (playerParty->containsCharacter(killer))
  {
    const unsigned int xp         = victim->getXpValue();
    const QString      victimName = victimSheet->getName();

    playerParty->grantXp(xp);
    displayConsoleMessage("You killed " + victimName + " and earned " + QString::number(xp) + " experience points.");
  }
}

void LevelTask::onPauseChanged()
{
  qDebug() << "LevelTask::onPauseChanged:" << paused;
  if (paused)
    updateTimer.stop();
  else
  {
    updateTimer.start();
    clock.restart();
  }
}

void LevelTask::advanceTime(unsigned int minutes)
{
  qint64 delta = minutes * 60 * 1000;

  for (DynamicObject* object : qAsConst(objects))
  {
    object->update(delta);
    object->getTaskManager()->update(delta);
    if (object->isCharacter())
      reinterpret_cast<Character*>(object)->getActionQueue()->update();
  }
  taskRunner->update(delta);
}

void LevelTask::update()
{
  qint64 delta = clock.restart();

  if (Game::get()->property("isGameEditor").toBool())
  {

  }
  else if (!combat)
  {
    timeManager->addElapsedMilliseconds(delta);
    for (DynamicObject* object : qAsConst(objects))
    {
      Character* asCharacter = reinterpret_cast<Character*>(object);

      object->update(delta);
      object->updateTasks(delta);
      if (object->isCharacter() && asCharacter->isAlive())
      {
        asCharacter->getFieldOfView()->update(delta);
        asCharacter->getActionQueue()->update();
      }
    }
    taskRunner->update(delta);
    Game::get()->getTaskManager()->update(delta);
  }
  else
  {
    for (DynamicObject* object : qAsConst(objects))
      object->update(delta);
    if (combattants.size() > combatIterator)
    {
      auto* combattant = combattants.at(combatIterator);

      if (combattant->getActionPoints() > 0 || !combattant->getActionQueue()->isEmpty())
        combattant->getActionQueue()->update();
      else
        onNextCombatTurn();
    }
    else
      throw std::runtime_error("combattants.size() > combatIterator");
  }
  updateVisualEffects(delta);
  soundManager->update();
  CombatComponent::update(delta);
  emit updated();
}

void LevelTask::onCombatChanged()
{
  for (auto* object : qAsConst(objects))
  {
    if (object->isCharacter())
    {
      auto* actionQueue = reinterpret_cast<Character*>(object)->getActionQueue();

      if (combat)
        actionQueue->pause();
      else
        actionQueue->unpause();
    }
  }
}

void LevelTask::finalizeRound()
{
  for (DynamicObject* object : qAsConst(objects))
  {
    Character* asCharacter = reinterpret_cast<Character*>(object);

    if (!object->isCharacter() || (!isInCombat(asCharacter) && asCharacter->isAlive()))
      object->getTaskManager()->update(WORLDTIME_TURN_DURATION);
    if (object->isCharacter() && object != getPlayer())
      asCharacter->getFieldOfView()->update(WORLDTIME_TURN_DURATION);
    taskRunner->update(WORLDTIME_TURN_DURATION);
    Game::get()->getTaskManager()->update(WORLDTIME_TURN_DURATION);
  }
  CombatComponent::finalizeRound();
}

Character* LevelTask::generateCharacter(const QString &name, const QString &characterSheet)
{
  Character* object = new Character(this);
  object->setObjectName(name);
  object->setCharacterSheet(characterSheet);
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

InventoryItem* LevelTask::generateInventoryItem(const QString& name, const QString& type, int quantity)
{
  InventoryItem* object = new InventoryItem(this);

  object->setObjectName(name);
  object->setItemType(type);
  if (quantity > 1)
    object->add(quantity - 1);
  registerDynamicObject(object);
  return object;
}

Doorway* LevelTask::generateDoorway(const QString &name)
{
  Doorway* object = new Doorway(this);

  object->setObjectName(name);
  registerDynamicObject(object);
  return object;
}

QQmlListProperty<Character> LevelTask::getQmlVisibleCharacters()
{
  if (Game::get()->property("isGameEditor").toBool())
  {
    visibleCharacters.clear();
    for (auto* object : qAsConst(objects))
    {
      if (object->isCharacter())
        visibleCharacters << reinterpret_cast<Character*>(object);
    }
  }
  else
  {
    visibleCharacters = getPlayer()->getFieldOfView()->GetDetectedCharacters();
    visibleCharacters << getPlayer();
  }
  return QQmlListProperty<Character>(this, &visibleCharacters);
}
