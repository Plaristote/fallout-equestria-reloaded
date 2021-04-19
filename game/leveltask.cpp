#include "leveltask.h"
#include "game.h"
#include "characterdialog.h"
#include <QJsonArray>
#include "characters/actionqueue.h"
#include "objects/inventoryitem.h"
#include "objects/doorway.h"
#include "game/animationSequence/movementhintanimationpart.h"
#include <QDebug>

LevelTask::LevelTask(QObject *parent) : ParentType(parent)
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
  ParentType::deleteLater();
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
  ParentType::load();
  grid->initializeGrid(tilemap);
  script = new ScriptController(SCRIPTS_PATH + "levels/" + levelName + ".mjs");
  taskRunner->setScriptController(script);
  script->initialize(this);
  for (auto* zone : tilemap->getZones())
    registerZone(zone);
  if (dataEngine->isLevelActive(name))
    loadObjectsFromDataEngine(dataEngine);
  if (!initialized && script && script->hasMethod("initialize"))
  {
    script->call("initialize");
    initialized = true;
  }
}

void LevelTask::loadObjectsFromDataEngine(DataEngine* dataEngine)
{
  QJsonObject levelData = dataEngine->getLevelData(name);
  QJsonValue  lastUpdate = levelData["lastUpdate"];

  StorableObject::load(levelData);
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
  initialized = levelData["tasks"].toBool(false);
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

  for (DynamicObject* object : qAsConst(objects))
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
  if (!isGameEditor())
  {
    levelData["init"] = initialized;
    levelData["lastUpdate"] = static_cast<int>(game->getTimeManager()->getDateTime().GetTimestamp());
    StorableObject::save(levelData);
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
    auto* actions = getPlayer()->getActionQueue();
    QPoint oldTarget(-1, -1);

    if (getPlayer()->getCurrentPath().length() > 0)
      oldTarget = getPlayer()->getCurrentPath().last();
    actions->reset();
    if (occupant)
      actions->pushReach(occupant, 1);
    else
      actions->pushMovement(QPoint(x, y));
    if (!(actions->start()))
      emit displayConsoleMessage("No path.");
    else if (!getPlayer()->getCurrentPath().empty())
    {
      if (oldTarget == getPlayer()->getCurrentPath().last())
        getPlayer()->setMovementMode("running");
      else if (oldTarget == QPoint(-1, -1))
        getPlayer()->setMovementMode("walking");
      displayMovementTargetHint(getPlayer()->getCurrentPath().last());
    }
  }
  else
  {
    emit clickedOnCase(x, y);
    emit clickedOnObject(occupant);
  }
}

void LevelTask::displayMovementTargetHint(QPoint position)
{
  AnimationSequence* animation = new AnimationSequence;

  animation->addAnimationPart(new MovementHintAnimationPart(position));
  addAnimationSequence(animation);
}

void LevelTask::registerDynamicObject(DynamicObject* object)
{
  if (object->isCharacter())
  {
    Character* character = reinterpret_cast<Character*>(object);

    addCharacterObserver(character, connect(character, &Character::itemDropped, [this, character](InventoryItem* item) { onItemDropped(item, character->getPosition()); }));
    addCharacterObserver(character, connect(character, &Character::characterKill, this, &LevelTask::onCharacterKill));
  }

  connect(object, &DynamicObject::lightZoneAdded, tilemap, &TileMap::addLightLayer);
  connect(object, &DynamicObject::lightZoneRemoved, tilemap, &TileMap::removeLightLayer);
  object->reloadLightzone();

  ParentType::registerDynamicObject(object);
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

  disconnect(object, &DynamicObject::lightZoneAdded, tilemap, &TileMap::addLightLayer);
  disconnect(object, &DynamicObject::lightZoneRemoved, tilemap, &TileMap::removeLightLayer);

  ParentType::unregisterDynamicObject(object);
}

void LevelTask::onItemDropped(InventoryItem* item, QPoint position)
{
  item->setParent(this);
  item->setPosition(position);
  registerDynamicObject(item);
  setObjectPosition(item, position.x(), position.y());
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
    object->updateTasks(delta);
    if (object->isCharacter())
      reinterpret_cast<Character*>(object)->getActionQueue()->update();
  }
  taskRunner->update(delta);
}

void LevelTask::update()
{
  qint64 delta = clock.restart();

  if (!combat)
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
  ParentType::update(delta);
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
    {
      qDebug() << "Update field of view for " << asCharacter->getStatistics()->getName();
      asCharacter->getFieldOfView()->update(WORLDTIME_TURN_DURATION);
      qDebug() << "-> Enemy count" << asCharacter->getFieldOfView()->GetDetectedEnemies().length();
    }
    taskRunner->update(WORLDTIME_TURN_DURATION);
    Game::get()->getTaskManager()->update(WORLDTIME_TURN_DURATION);
  }
  ParentType::finalizeRound();
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

DynamicObject* LevelTask::generateDynamicObject(const QString &name)
{
  DynamicObject* object = new DynamicObject(this);

  object->setObjectName(name);
  registerDynamicObject(object);
  return object;
}

QVariantList LevelTask::previewPathTo(int x, int y)
{
  auto* grid  = getGrid();
  QList<QPoint> path;
  QVariantList result;

  if (getPlayer() && grid->findPath(getPlayer()->getPosition(), QPoint(x, y), path))
  {
    for (const auto& point : qAsConst(path))
      result.push_back(point);
  }
  return result;
}
