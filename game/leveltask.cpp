#include "leveltask.h"
#include "game.h"
#include "characterdialog.h"
#include <QJsonArray>
#include "inventoryitem.h"
#include "characters/actionqueue.h"

LevelTask::LevelTask(QObject *parent) : CombatComponent(parent)
{
  tilemap = new TileMap(this);
  soundManager = new SoundManager(this);
  updateTimer.setInterval(15);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &LevelTask::update);
  connect(this, &LevelTask::pausedChanged, this, &LevelTask::onPauseChanged);
}

LevelTask::~LevelTask()
{
  qDebug() << "LevelTask::destroyed";
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
  tilemap->load(levelName);
  grid->initializeGrid(tilemap);

  if (dataEngine->isLevelActive(name))
    loadObjectsFromDataEngine(dataEngine);
  for (auto* zone : tilemap->getZones())
  {
    connect(zone, &TileZone::enteredZone, this, &LevelTask::onZoneEntered, Qt::QueuedConnection);
    connect(zone, &TileZone::exitedZone,  this, &LevelTask::onZoneExited,  Qt::QueuedConnection);
  }
}

void LevelTask::loadObjectsFromDataEngine(DataEngine* dataEngine)
{
  QJsonObject levelData = dataEngine->getLevelData(name);

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
    if (occupant)
    {
      QPoint interactionPosition = occupant->getInteractionPosition();
      x = interactionPosition.x();
      y = interactionPosition.y();
    }
    if (!moveTo(getPlayer(), x, y))
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

void LevelTask::registerVisualEffect(Sprite* sprite)
{
  visualEffects << sprite;
  emit visualEffectsChanged();
}

void LevelTask::unregisterVisualEffect(Sprite* sprite)
{
  visualEffects.removeAll(sprite);
  emit visualEffectsChanged();
}

void LevelTask::registerControlZone(TileZone* zone)
{
  tilemap->addTileZone(zone);
}

void LevelTask::unregisterControlZone(TileZone* zone)
{
  tilemap->removeTileZone(zone);
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

  if (!combat)
  {
    timeManager->addElapsedMilliseconds(delta);
    for (DynamicObject* object : objects)
    {
      object->update(delta);
      object->getTaskManager()->update(delta);
      if (object->isCharacter())
        reinterpret_cast<Character*>(object)->getActionQueue()->update();
    }
  }
  else
  {
    for (DynamicObject* object : objects)
      object->update(delta);
    if (combattants.size() > combatIterator)
    {
      auto* combattant = combattants.at(combatIterator);

      if (combattant->getActionPoints() > 0)
        combattant->getActionQueue()->update();
      else
        onNextCombatTurn();
    }
  }
  soundManager->update();
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
