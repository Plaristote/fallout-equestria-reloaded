#include "uniquecharacterstorage.h"

UniqueCharacterStorage::UniqueCharacterStorage(QObject *parent)
  : QObject{parent}
{

}

int UniqueCharacterStorage::loadUniqueCharactersToLevel(GridComponent* level)
{
  if(level == nullptr)
  {
    qDebug()<<"Level pointer is null.";
    return -1;
  }

  int numberOfCharactersLoaded = 0;



  return numberOfCharactersLoaded;

}

int UniqueCharacterStorage::saveUniqueCharactersFromLevel(GridComponent* level)
{
  if(level == nullptr)
  {
    qDebug()<<"Level pointer is null.";
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
        StorageSlot* slot = new StorageSlot(this,character,position);

        storage.append(slot);

        numberOfCharactersSaved++;
      }else
      {
        qDebug()<<"Could not detach dynamic object "<<dynamicObject->getObjectName()<<" from level "<<level->getName();
      }
    }
  }

  QString levelName = level->getName();
  levelToStorage[levelName] = storage;

  return numberOfCharactersSaved;
}

StorageSlot::StorageSlot(QObject *parent, Character* character, QPoint position)
  : QObject{parent}
{
  storedCharacter = character;
  storedPosition = position;
}
