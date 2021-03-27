#include "quest.h"
#include <QJsonObject>
#include "game.h"

Quest::Quest(QObject *parent) : QObject(parent)
{
}

Quest::~Quest()
{
  if (script)
    delete script;
}

void Quest::initialize(const QString& name)
{
  this->name = name;
  completed = false;
  qDebug() << "INITIALIZING QUEST" << (SCRIPTS_PATH + "quests/" + name + ".mjs");
  script = new ScriptController(SCRIPTS_PATH + "quests/" + name + ".mjs");
  script->initialize(this);
  if (script->hasMethod("onQuestStart"))
    script->call("onQuestStart");
}

void Quest::load(const QJsonObject& data)
{
  name = data["name"].toString();
  variables = data["vars"].toObject().toVariantMap();
  completed = data["over"].toBool(false);
  script = new ScriptController(SCRIPTS_PATH + "/quests/" + name + ".mjs");
  script->initialize(this);
}

QJsonObject Quest::save() const
{
  QJsonObject data;

  data.insert("name", name);
  data.insert("vars", QJsonObject::fromVariantMap(variables));
  data.insert("over", completed);
  return data;
}

void Quest::completeObjective(const QString& name)
{
  if (script)
    script->call("completeObjective", QJSValueList() << name);
}

bool Quest::isObjectiveCompleted(const QString& name) const
{
  if (script)
    return script->call("isObjectiveCompleted", QJSValueList() << name).toBool();
  return false;
}

void Quest::onCharacterKilled(Character* character, Character* killer)
{
  if (!completed && script->hasMethod("onCharacterKilled"))
  {
    QJSValueList args;
    auto& scriptEngine = Game::get()->getScriptEngine();

    args << character->asJSValue();
    if (killer)
      args << killer->asJSValue();
    script->call("onCharacterKilled", args);
  }
}

void Quest::onItemPicked(InventoryItem* item)
{
  if (!completed && item && script->hasMethod("onItemPicked"))
    script->call("onItemPicked", QJSValueList() << item->asJSValue());
}

QVariantList Quest::getObjectives() const
{
  if (script->hasMethod("getObjectives"))
    return script->call("getObjectives").toVariant().toList();
  else
    qDebug() << "Quest" << name << ": missing `getObjectives` method";
  return QVariantList();
}
