#include "objectfactory.h"
#include "../character.h"
#include "storageobject.h"
#include "inventoryitem.h"
#include "doorway.h"

ObjectFactory::ObjectFactory(ObjectGroup* root) : QObject(root), root(root)
{

}

Character* ObjectFactory::generateCharacter(const QString &name, const QString &characterSheet)
{
  Character* object = new Character(root);
  object->setObjectName(name);
  object->setCharacterSheet(characterSheet);
  root->appendObject(object);
  return object;
}

StorageObject* ObjectFactory::generateStorageObject(const QString &name)
{
  StorageObject* object = new StorageObject(root);

  object->setObjectName(name);
  root->appendObject(object);
  return object;
}

InventoryItem* ObjectFactory::generateInventoryItem(const QString& name, const QString& type, int quantity)
{
  InventoryItem* object = new InventoryItem(root);

  object->setObjectName(name);
  object->setItemType(type);
  if (quantity > 1)
    object->add(quantity - 1);
  root->appendObject(object);
  return object;
}

Doorway* ObjectFactory::generateDoorway(const QString &name)
{
  Doorway* object = new Doorway(root);

  object->setObjectName(name);
  root->appendObject(object);
  return object;
}

DynamicObject* ObjectFactory::generateDynamicObject(const QString &name)
{
  DynamicObject* object = new DynamicObject(root);

  object->setObjectName(name);
  root->appendObject(object);
  return object;
}
