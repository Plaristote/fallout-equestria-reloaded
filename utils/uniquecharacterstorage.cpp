#include "uniquecharacterstorage.h"

UniqueCharacterStorage::UniqueCharacterStorage(QObject *parent)
    : QObject{parent}
{

}

int UniqueCharacterStorage::saveUniqueCharactersFromLevel(GridComponent* level)
{
  if(level == nullptr)
  {
    qDebug()<<"Level pointer is null.";
    return -1;
  }

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
        numberOfCharactersSaved++;
      }else
      {
        qDebug()<<"Could not detach dynamic object "<<dynamicObject->getObjectName()<<" from level "<<level->getName();
      }
    }
  }

  return numberOfCharactersSaved;
}
