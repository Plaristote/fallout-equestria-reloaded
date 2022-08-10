#include "uniquecharacterstorage.h"
#include "game.h"

UniqueCharacterStorage::UniqueCharacterStorage(QObject *parent)
  : QObject{parent}
{
  qDebug()<<"UniqueCharacterStorage: Seting up storage for unique characters.";
}

int UniqueCharacterStorage::loadUniqueCharactersToLevel(GridComponent* level)
{
  qDebug()<<"UniqueCharacterStorage: Load unique characters to "<<level->getName()<<".";
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
    QPoint position = slot->storedPosition;

    long timeAtStorage = slot->storedTimestampAtStorage;
    qint64 elapsedTime = (currentTime - timeAtStorage) * 1000; // it needs milliseconds

    level->appendObject(character);
    level->setCharacterPosition(character, position.x(), position.y());
    character->getTaskManager()->update(elapsedTime);
  }

  // TODO: should I eliminate manually all the slots?

  return numberOfCharactersLoaded;

}

void UniqueCharacterStorage::log()
{
  QList<QString> levels = levelToStorage.keys();
  qDebug()<<"UniqueCharacterStorage: Levels "<<levels.count()<<".";

  for(QString level : levels)
  {
    qDebug()<<"UniqueCharacterStorage: Level "<<level<<".";
    QList<StorageSlot*> characterSlots = levelToStorage[level];
    for (auto slot : characterSlots)
    {
      qDebug()<<slot->storedCharacter->getDialogName();
      qDebug()<<"UniqueCharacterStorage: x: "<<slot->storedPosition.x()<<", y: "<<slot->storedPosition.y();
      qDebug()<<"UniqueCharacterStorage: Time stamp: "<<slot->storedTimestampAtStorage;
    }
  }
}

int UniqueCharacterStorage::saveUniqueCharactersFromLevel(GridComponent* level)
{
  qDebug()<<"UniqueCharacterStorage: Saving unique characters from "<<level->getName()<<".";
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

    if(dynamicObject->isCharacter() && ((Character)dynamicObject).getIsUnique())
    {
      bool isDetached = level->detachObject(dynamicObject);
      if(isDetached)
      {

        dynamicObject->setParent(this);
        Character* character = (Character*)dynamicObject;
        QPoint position = character->getPosition();
        TimeManager* tm = Game::get()->getTimeManager();
        long time = tm->getTimestamp();
        StorageSlot* slot = new StorageSlot(this,character,position,time);

        storage.append(slot);

        numberOfCharactersSaved++;
      }else
      {
        qDebug()<<"UniqueCharacterStorage: Could not detach dynamic object "<<dynamicObject->getObjectName()<<" from level "<<level->getName();
      }
    }
  }

  QString levelName = level->getName();
  levelToStorage[levelName] = storage;

  return numberOfCharactersSaved;
}

StorageSlot::StorageSlot(QObject *parent, Character* character, QPoint position, long timestampAtStorage)
  : QObject{parent}
{
  storedCharacter = character;
  storedPosition = position;
  storedTimestampAtStorage = timestampAtStorage;
}
