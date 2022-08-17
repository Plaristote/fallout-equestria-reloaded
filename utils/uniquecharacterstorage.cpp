#include "uniquecharacterstorage.h"
#include "game.h"
#include <QJsonObject>
#include <QJsonArray>

UniqueCharacterStorage::UniqueCharacterStorage(QObject *parent)
  : QObject{parent}
{
  qDebug()<<"UniqueCharacterStorage: Seting up storage for unique characters.";
}

int UniqueCharacterStorage::loadUniqueCharactersToLevel(GridComponent* level)
{
  qDebug()<<"UniqueCharacterStorage: Load unique characters to"<<level->getName()<<".";
  if(level == nullptr)
  {
    qDebug()<<"UniqueCharacterStorage: Level pointer is null.";
    return -1;
  }

  QString levelName = level->getName();
  QList<StorageSlot*> storage = levelToStorage.take(levelName);
  int numberOfCharactersLoaded = storage.count();
  TimeManager* timeManager = Game::get()->getTimeManager();
  long currentTime = timeManager->getTimestamp();

  for(int i=0; i<storage.count();i++)
  {
    StorageSlot* slot = storage.at(i);

    Character* character = slot->storedCharacter;
    Point position = character->getPoint();

    long timeAtStorage = slot->storedTimestampAtStorage;
    qint64 elapsedTime = (currentTime - timeAtStorage) * 1000; // it needs milliseconds

    level->appendObject(character);
    level->setCharacterPosition(character, position.x, position.y, position.z);
    character->getTaskManager()->update(elapsedTime);
  }

  for (auto slot: storage)
  {
    slot->deleteLater();
    storage.clear();
  }

  return numberOfCharactersLoaded;

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

int UniqueCharacterStorage::saveUniqueCharactersFromLevel(GridComponent* level)
{
  qDebug()<<"UniqueCharacterStorage: Saving unique characters from"<<level->getName()<<".";
  if(level == nullptr)
  {
    qDebug()<<"UniqueCharacterStorage: Level pointer is null.";
    return -1;
  }

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
        qDebug()<<"UniqueCharacterStorage: Could not detach dynamic object"<<dynamicObject->getObjectName()<<" from level "<<level->getName();
      }
    }
  }

  QString levelName = level->getName();
  levelToStorage[levelName] = storage;

  return numberOfCharactersSaved;
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
