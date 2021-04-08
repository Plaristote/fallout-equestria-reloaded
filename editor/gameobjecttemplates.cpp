#include "gameobjecttemplates.h"
#include "game/dynamicobject.h"
#include "game/character.h"
#include "game/objects/doorway.h"
#include "globals.h"
#include <QJsonDocument>

#define TYPE_BUILDER(TYPE) \
  [](const QJsonObject& data) { TYPE* value = new TYPE; value->load(data); return value; }

GameObjectTemplates* GameObjectTemplates::instance = nullptr;

const QMap<QString, std::function<DynamicObject* (const QJsonObject&)>> GameObjectTemplates::typeMap = {
  {"Character",     TYPE_BUILDER(Character)},
  {"StorageObject", TYPE_BUILDER(StorageObject)},
  {"Doorway",       TYPE_BUILDER(Doorway)},
  {"DynamicObject", TYPE_BUILDER(DynamicObject)}
};

GameObjectTemplates::GameObjectTemplates(QObject *parent) : QObject(parent)
{
  instance = this;
}

void GameObjectTemplates::initialize()
{
  QFile file(ASSETS_PATH + "templates.json");

  if (file.open(QIODevice::ReadOnly))
  {
    data = QJsonDocument::fromJson(file.readAll()).object();
    file.close();
    emit namesChanged();
  }
}

void GameObjectTemplates::save()
{
  QFile file(ASSETS_PATH + "templates.json");

  if (file.open(QIODevice::WriteOnly))
  {
    file.write(QJsonDocument(data).toJson());
    file.close();
  }
}

void GameObjectTemplates::save(const QString &name, DynamicObject* object)
{
  QJsonObject objectData;

  object->save(objectData);
  objectData["_type"] = object->getObjectType();
  data.insert(name, objectData);
  save();
}

DynamicObject* GameObjectTemplates::load(const QString &name)
{
  DynamicObject* object = nullptr;
  QJsonObject    objectData = data[name].toObject();
  QString        typeName = objectData["_type"].toString();
  auto           it = typeMap.find(typeName);

  if (it != typeMap.end())
    return (*it)(objectData);
  return object;
}
