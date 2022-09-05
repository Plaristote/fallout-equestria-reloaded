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

  QList<StorageSlot*> storage;
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

    if(isUnique)
    {
      bool isDetached = level->detachObject(dynamicObject);
      if(isDetached)
      {
        character->setParent(this);
        TimeManager* tm = Game::get()->getTimeManager();
        long time = tm->getTimestamp();
        character->getFieldOfView()->reset();

        StorageSlot* slot = new StorageSlot(this,character,time);
        storage.append(slot);

        numberOfCharactersSaved++;
      }else
      {
        qDebug()<<"UniqueCharacterStorage: Could not detach dynamic object"<<dynamicObject->getObjectName()<<"from level"<<level->getName();
      }
    }
  }

  QString levelName = level->getName();
  levelToStorage[levelName] = storage;

  return numberOfCharactersSaved;
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

bool UniqueCharacterStorage::loadCharacterToCurrentLevel(QString characterSheet, int x = 0, int y = 0, int z = 0)
{
  qDebug()<<"UniqueCharacterStorage: Looking for character:"<<characterSheet;
  bool character_loaded = false;

  // find the character first
  StorageSlot* characterSlot = nullptr;
  bool character_found = false;
  auto keys = levelToStorage.keys();

  for(int i = 0; i<keys.count() && !character_found; i++)
  {
    QList<StorageSlot*>& storage = levelToStorage[keys.at(i)];

    for(int j = 0; j<storage.count() && !character_found; j++)
    {
      StorageSlot* slot = storage.at(j);

      if(characterSheet == slot->storedCharacter->getCharacterSheet())
      {
        characterSlot = slot;
        character_found = true;
        storage.removeAll(characterSlot);
      }
    }
  }
  // if found
  if(character_found)
  {
    LevelTask* level = Game::get()->getLevel();
    Point position = Point();
    position.x = x;
    position.y = y;
    position.z = z;

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
