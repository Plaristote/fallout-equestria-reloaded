#include "levelbase.h"
#include "game.h"

LevelBase::LevelBase(QObject *parent) : StorableObject(parent)
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

DynamicObject* LevelBase::getObjectByName(const QString &name) const
{
  for (DynamicObject* object : objects)
  {
    if (object->getObjectName() == name)
      return object;
  }
  return nullptr;
}

QList<Character*> LevelBase::findCharacters(std::function<bool (Character &)> compare) const
{
  QList<Character*> characters;

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

void LevelBase::registerDynamicObject(DynamicObject* object)
{
  objects.push_back(object);
  emit objectsChanged();
}

void LevelBase::unregisterDynamicObject(DynamicObject* object)
{
  objects.removeAll(object);
  emit objectsChanged();
}
