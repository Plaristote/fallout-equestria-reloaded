#include "uniquecharacterstorage.h"
#include "game.h"
#include <QJsonObject>
#include <QJsonArray>

UniqueCharacterStorage::UniqueCharacterStorage(QObject *parent)
  : QObject{parent}
{
  qDebug()<<"UniqueCharacterStorage: Seting up storage for unique characters.";
}

int UniqueCharacterStorage::saveUniqueCharactersFromLevel(LevelTask* level)
{
  if(level == nullptr)
  {
    qDebug()<<"UniqueCharacterStorage: Level pointer is null.";
    return -1;
  }
  qDebug()<<"UniqueCharacterStorage: Saving unique characters from"<<level->getName()<<".";

  QList<StorageSlot*>& storage = requireLevelStorage(level->getName());
  int numberOfCharactersSaved = 0;

  QVector<DynamicObject*> dynamicObjects = level->allDynamicObjects();
  for(int i = 0; i<dynamicObjects.count(); i++)
  {
    DynamicObject* dynamicObject = dynamicObjects.at(i);
    bool isCharacter = dynamicObject->isCharacter();
    if(!isCharacter)
      continue;

    Character* character = (Character*)dynamicObject;
    bool isUnique = character->getIsUnique();

    if(isUnique && saveCharacterIntoStorage(level, character, storage))
      numberOfCharactersSaved++;
  }

  return numberOfCharactersSaved;
}

bool UniqueCharacterStorage::saveCharacterIntoStorage(LevelTask* level, Character* character, QList<StorageSlot*>& storage)
{
  bool isDetached = level->detachObject(character);

  if(isDetached)
  {
    character->setParent(this);
    TimeManager* tm = Game::get()->getTimeManager();
    long time = tm->getTimestamp();
    character->getFieldOfView()->reset();

    StorageSlot* slot = new StorageSlot(this,character,time);
    storage.append(slot);
  }else
  {
    qDebug()<<"UniqueCharacterStorage: Could not detach dynamic object"<<character->getObjectName()<<"from level"<<level->getName();
  }
  return isDetached;
}

QList<StorageSlot*>& UniqueCharacterStorage::requireLevelStorage(const QString& levelName)
{
  auto iterator = levelToStorage.find(levelName);

  if (iterator == levelToStorage.end())
    iterator = levelToStorage.insert(levelName, {});
  return *iterator;
}

void UniqueCharacterStorage::saveCharacterFromCurrentLevel(Character* character)
{
  LevelTask* level = Game::get()->getLevel();

  if (level)
  {
    saveCharacterIntoStorage(
      level,
      character,
      requireLevelStorage(level->getName())
    );
  }
}

int UniqueCharacterStorage::loadUniqueCharactersToLevel(LevelTask* level)
{
  if(level == nullptr)
  {
    qDebug()<<"UniqueCharacterStorage: Level pointer is null.";
    return -1;
  }
  qDebug()<<"UniqueCharacterStorage: Load unique characters to"<<level->getName()<<".";

  QString levelName = level->getName();
  QList<StorageSlot*> storage = levelToStorage.take(levelName);
  int numberOfCharactersLoaded = storage.count();

  for(int i=0; i<storage.count();i++)
  {
    loadCharacterIntoLevel(level, storage.at(i));
  }

  for (auto slot: storage)
  {
    slot->deleteLater();
  }
  storage.clear();

  return numberOfCharactersLoaded;
}

Character* UniqueCharacterStorage::getCharacter(const QString& characterSheet)
{
  StorageSlot* slot = getCharacterSlot(characterSheet);

  return slot ? slot->storedCharacter : nullptr;
}

StorageSlot* UniqueCharacterStorage::getCharacterSlot(const QString& characterSheet, bool take)
{
  for (auto it = levelToStorage.begin() ; it != levelToStorage.end() ; ++it)
  {
    QList<StorageSlot*>& storage = *it;

    for(int j = 0; j<storage.count() ; j++)
    {
      StorageSlot* slot = storage.at(j);

      if(characterSheet == slot->storedCharacter->getCharacterSheet())
      {
        if (take)
          storage.removeAll(slot);
        return slot;
      }
    }
  }

  return nullptr;
}

bool UniqueCharacterStorage::loadCharacterToCurrentLevel(const QString& characterSheet, int x, int y, int z)
{
  qDebug()<<"UniqueCharacterStorage: Looking for character:"<<characterSheet;
  bool character_loaded = false;

  // find the character first
  StorageSlot* characterSlot = takeCharacterSlot(characterSheet);
  // if found
  if(characterSlot)
  {
    LevelTask* level = Game::get()->getLevel();
    Point position = Point();
    position.x = x;
    position.y = y;
    position.z = z == NULL_FLOOR ? level->getCurrentFloor() : z;

    loadCharacterIntoLevel(level, characterSlot, position);
    characterSlot->deleteLater();
    character_loaded = true;
    qDebug()<<"UniqueCharacterStorage: character loaded.";
  }else{
    qDebug()<<"UniqueCharacterStorage: character not found.";
  }

  return character_loaded;
}

void UniqueCharacterStorage::log()
{
  QList<QString> levels = levelToStorage.keys();
  qDebug()<<"UniqueCharacterStorage: Levels"<<levels.count()<<".";

  for(QString level : levels)
  {
    qDebug()<<"UniqueCharacterStorage: Level"<<level<<"with"<<levelToStorage[level].count()<<"characters";
    QList<StorageSlot*> characterSlots = levelToStorage[level];
    for (auto slot : characterSlots)
    {
      qDebug()<<"UniqueCharacterStorage:"<<slot->storedCharacter->getBaseName();
      qDebug()<<"UniqueCharacterStorage: Time stamp"<<slot->storedTimestampAtStorage;
    }
  }
}

bool UniqueCharacterStorage::loadCharacterIntoLevel(LevelTask *level, StorageSlot *characterSlot)
{
  Point position = characterSlot->storedCharacter->getPoint();
  return loadCharacterIntoLevel(level, characterSlot, position);
}

bool UniqueCharacterStorage::loadCharacterIntoLevel(LevelTask *level, StorageSlot *characterSlot, Point position)
{
  bool character_loaded = false;

  Character* character = characterSlot->storedCharacter;

  TimeManager* timeManager = Game::get()->getTimeManager();
  long currentTime = timeManager->getTimestamp();
  long timeAtStorage = characterSlot->storedTimestampAtStorage;
  qint64 elapsedTime = (currentTime - timeAtStorage) * 1000; // it needs milliseconds

  level->appendObject(character);
  level->setCharacterPosition(character, position.x, position.y, position.z);
  character->getTaskManager()->update(elapsedTime);

  return character_loaded;
}


void UniqueCharacterStorage::load(const QJsonObject& data)
{
  for (auto it = data.begin() ; it != data.end() ; ++it)
  {
    QList<StorageSlot*> slotList;

    for (auto slotJson : it.value().toArray())
    {
      StorageSlot* slot = new StorageSlot(this);

      slot->load(slotJson.toObject());
      slotList.push_back(slot);
    }
    levelToStorage[it.key()] = slotList;
  }
}

void UniqueCharacterStorage::save(QJsonObject& data)
{
  for (auto it = levelToStorage.begin() ; it != levelToStorage.end() ; ++it)
  {
    QJsonArray storageSlotsJson;

    for (StorageSlot* slot : it.value())
    {
      QJsonObject slotJson;

      slot->save(slotJson);
      storageSlotsJson.push_back(slotJson);
    }
    data[it.key()] = storageSlotsJson;
  }
}

StorageSlot::StorageSlot(QObject *parent, Character* character, long timestampAtStorage)
  : QObject{parent}
{
  storedCharacter = character;
  storedTimestampAtStorage = timestampAtStorage;
}

void StorageSlot::load(const QJsonObject& data)
{
  storedCharacter = new Character(parent());
  storedCharacter->load(data["character"].toObject());
  storedTimestampAtStorage = data["timestamp"].toVariant().toLongLong();
}

void StorageSlot::save(QJsonObject& data)
{
  QJsonObject characterJson;

  storedCharacter->save(characterJson);
  data["character"] = characterJson;
  data["timestamp"] = QJsonValue::fromVariant(QVariant::fromValue(storedTimestampAtStorage));
}
