#include "levelbase.h"
#include "game.h"

static bool freezeObjectRegistration = false;

LevelBase::LevelBase(QObject *parent) : ObjectGroup(parent)
{
  onChildrenGroupAdded(this);
}

bool LevelBase::isGameEditor() const
{
  return Game::get()->getIsGameEditor();
}

QString LevelBase::getScriptFilename(const QString& levelName) const
{
  QString defaultName = levelName + ".mjs";

  if (QFileInfo(getScriptPath() + '/' + defaultName).isFile())
    return levelName + ".mjs";
  return "base.mjs";
}

Character* LevelBase::getPlayer() const
{
  return Game::get()->getPlayer();
}

QList<Character*> LevelBase::findCharacters(std::function<bool (Character &)> compare) const
{
  QList<Character*> characters;

  characters.reserve(512);
  findDynamicObjects([compare, &characters](DynamicObject& object)
  {
    if (object.isCharacter() && compare(reinterpret_cast<Character&>(object)))
      characters << reinterpret_cast<Character*>(&object);
    return false;
  });
  return characters;
}

Character* LevelBase::findCharacter(std::function<bool (Character&)> compare) const
{
  return reinterpret_cast<Character*>(
    findObject([compare](DynamicObject& object)
    {
      return object.isCharacter() && compare(reinterpret_cast<Character&>(object));
    })
  );
}

void LevelBase::load(const QJsonObject& data)
{
  freezeObjectRegistration = true;
  ObjectGroup::load(data);
  freezeObjectRegistration = false;
}

void LevelBase::onChildrenGroupAdded(ObjectGroup* group)
{
  registerZoneController(group);
  connect(group, &ObjectGroup::objectAdded,   this, &LevelBase::onChildrenObjectAdded);
  connect(group, &ObjectGroup::objectRemoved, this, &LevelBase::onChildrenObjectRemoved);
  connect(group, &ObjectGroup::groupAdded,    this, &LevelBase::onChildrenGroupAdded);
  connect(group, &ObjectGroup::groupRemoved,  this, &LevelBase::onChildrenGroupRemoved);
  for (ObjectGroup* subgroup : group->getGroups())
    onChildrenGroupAdded(subgroup);
  for (DynamicObject* object : group->getObjects())
    onChildrenObjectAdded(object);
}

void LevelBase::onChildrenGroupRemoved(ObjectGroup* group)
{
  unregisterZoneController(group);
  for (ObjectGroup* subgroup : group->getGroups())
    onChildrenGroupRemoved(subgroup);
  for (DynamicObject* object : group->getObjects())
    onChildrenObjectRemoved(object);
  disconnect(group, &ObjectGroup::objectAdded,   this, &LevelBase::onChildrenObjectAdded);
  disconnect(group, &ObjectGroup::objectRemoved, this, &LevelBase::onChildrenObjectRemoved);
  disconnect(group, &ObjectGroup::groupAdded,    this, &LevelBase::onChildrenGroupAdded);
  disconnect(group, &ObjectGroup::groupRemoved,  this, &LevelBase::onChildrenGroupRemoved);
}

void LevelBase::onChildrenObjectAdded(DynamicObject* object)
{
  if (!freezeObjectRegistration && !attachedObjects.contains(object))
    registerDynamicObject(object);
}

void LevelBase::onChildrenObjectRemoved(DynamicObject* object)
{
  if (attachedObjects.contains(object))
    unregisterDynamicObject(object);
}

void LevelBase::registerAllDynamicObjects()
{
  eachObject(std::bind(&LevelBase::registerDynamicObject, this, std::placeholders::_1));
}

void LevelBase::registerDynamicObject(DynamicObject* object)
{
  attachedObjects.push_back(object);
  emit attachedObjectsChanged();
}

void LevelBase::unregisterDynamicObject(DynamicObject* object)
{
  attachedObjects.removeOne(object);
  emit attachedObjectsChanged();
}
