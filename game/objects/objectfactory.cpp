#include "objectfactory.h"
#include "../character.h"
#include "storageobject.h"
#include "inventoryitem.h"
#include "bloodstain.h"
#include "doorway.h"
#include "game.h"

#define GAME_OBJECT_CONSTRUCTOR(TYPENAME) \
  {#TYPENAME, [](QObject* parent) { return new TYPENAME(parent); }}

static const QMap<QString, std::function<DynamicObject* (QObject*)>> constructors = {
  GAME_OBJECT_CONSTRUCTOR(DynamicObject),
  GAME_OBJECT_CONSTRUCTOR(Character),
  GAME_OBJECT_CONSTRUCTOR(StorageObject),
  GAME_OBJECT_CONSTRUCTOR(Doorway),
  GAME_OBJECT_CONSTRUCTOR(InventoryItem),
  GAME_OBJECT_CONSTRUCTOR(BloodStain)
};

ObjectFactory::ObjectFactory(ObjectGroup* root) : QObject(root), root(root)
{

}

DynamicObject* ObjectFactory::loadFromJson(const QJsonObject& data) const
{
  auto constructor = constructors.find(data["type"].toString());

  if (constructor != constructors.end())
  {
    DynamicObject* object = constructor.value()(root);

    object->load(data);
    object->setRenderPosition(object->getSpritePosition()); // isn't this basically self-assign ?
    root->appendObject(object);
  }
  else
    qDebug() << "/!\\ ObjectFactory: could not load object" << (root->getPath() + '.' + data["name"].toString()) << "of type" << data["type"].toString();
  return nullptr;
}

ObjectGroup* ObjectFactory::loadJsonGroup(const QJsonObject& data) const
{
  ObjectGroup* newGroup = new ObjectGroup(root);

  newGroup->load(data);
  root->appendGroup(newGroup);
  return newGroup;
}

ObjectGroup* ObjectFactory::generateGroup(const QString &name) const
{
  ObjectGroup* group = new ObjectGroup(root);

  group->setProperty("name", name);
  root->appendGroup(group);
  return group;
}

Character* ObjectFactory::generateCharacter(const QString &name, const QString &characterSheet) const
{
  Character* object = new Character(root);

  object->setObjectName(name);
  object->setCharacterSheet(characterSheet);
  root->appendObject(object);
  return object;
}

StorageObject* ObjectFactory::generateStorageObject(const QString &name) const
{
  StorageObject* object = new StorageObject(root);

  object->setObjectName(name);
  root->appendObject(object);
  return object;
}

InventoryItem* ObjectFactory::generateInventoryItem(const QString& name, const QString& type, int quantity) const
{
  InventoryItem* object = new InventoryItem(root);

  object->setObjectName(name);
  object->setItemType(type);
  if (quantity > 1)
    object->add(quantity - 1);
  root->appendObject(object);
  return object;
}

Doorway* ObjectFactory::generateDoorway(const QString &name) const
{
  Doorway* object = new Doorway(root);

  object->setObjectName(name);
  root->appendObject(object);
  return object;
}

DynamicObject* ObjectFactory::generateDynamicObject(const QString &name) const
{
  DynamicObject* object = new DynamicObject(root);

  object->setObjectName(name);
  root->appendObject(object);
  return object;
}

DynamicObject* ObjectFactory::addBloodStainAt(QPoint position) const
{
  BloodStain* object = new BloodStain(root);
  LevelTask* level = Game::get()->getLevel();
  QPoint offset = root->getPosition();

  root->appendObject(object);
  if (level)
    level->setObjectPosition(object, offset.x() + position.x(), offset.y() + position.y());
  object->initialize();
  return object;
}
