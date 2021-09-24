#include "levelbase.h"
#include "game.h"

static bool freezeObjectRegistration = false;

LevelBase::LevelBase(QObject *parent) : ObjectGroup(parent)
{

}

bool LevelBase::isGameEditor() const
{
  return Game::get()->property("isGameEditor").toBool();
}

Character* LevelBase::getPlayer() const
{
  return Game::get()->getPlayer();
}

QList<Character*> LevelBase::findCharacters(std::function<bool (Character &)> compare) const
{
  QList<Character*> characters;

  characters.reserve(objects.length());
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

void LevelBase::load(const QJsonObject& data)
{
  freezeObjectRegistration = true;
  ObjectGroup::load(data);
  freezeObjectRegistration = false;
}

void LevelBase::onChildrenGroupAdded(ObjectGroup* group)
{
  connect(group, &ObjectGroup::objectAdded,   this, &LevelBase::onChildrenObjectAdded);
  connect(group, &ObjectGroup::objectRemoved, this, &LevelBase::onChildrenObjectRemoved);
  connect(group, &ObjectGroup::groupAdded,    this, &LevelBase::onChildrenGroupAdded);
  connect(group, &ObjectGroup::groupRemoved,  this, &LevelBase::onChildrenGroupRemoved);
}

void LevelBase::onChildrenGroupRemoved(ObjectGroup* group)
{
  disconnect(group, &ObjectGroup::objectAdded,   this, &LevelBase::onChildrenObjectAdded);
  disconnect(group, &ObjectGroup::objectRemoved, this, &LevelBase::onChildrenObjectRemoved);
  disconnect(group, &ObjectGroup::groupAdded,    this, &LevelBase::onChildrenGroupAdded);
  disconnect(group, &ObjectGroup::groupRemoved,  this, &LevelBase::onChildrenGroupRemoved);
}

void LevelBase::onChildrenObjectAdded(DynamicObject* object)
{
  if (!freezeObjectRegistration)
    registerDynamicObject(object);
}

void LevelBase::onChildrenObjectRemoved(DynamicObject* object)
{
  unregisterDynamicObject(object);
}

void LevelBase::registerAllDynamicObjects()
{
  eachObject(std::bind(&LevelBase::registerDynamicObject, this, std::placeholders::_1));
}

void LevelBase::registerDynamicObject(DynamicObject*)
{
}

void LevelBase::unregisterDynamicObject(DynamicObject*)
{
}
