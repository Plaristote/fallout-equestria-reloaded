#include "quest.h"
#include <QJsonObject>
#include <QDebug>
#include "game.h"
#include "i18n.h"

static void playNewQuestSound()
{
  Game::get()->getSoundManager()->play("pipbuck/newquest");
}

Quest::Quest(QObject *parent) : StorableObject(parent)
{
  connect(this, &Quest::completedChanged, this, &Quest::onCompletedChanged);
  hidden = false;
}

Quest::~Quest()
{
  if (script)
    delete script;
}

void Quest::initialize(const QString& name, bool hidden)
{
  this->name = name;
  this->hidden = hidden;
  completed = failed = false;
  script = new ScriptController(SCRIPTS_PATH + "quests/" + name + ".mjs");
  script->initialize(this);
  if (script->hasMethod("initialize"))
    script->call("initialize");
  if (!hidden)
    playNewQuestSound();
}

void Quest::load(const QJsonObject& data)
{
  StorableObject::load(data);
  name = data["name"].toString();
  location = data["location"].toString();
  completed = data["over"].toBool(false);
  hidden = data["hidden"].isBool();
  failed = data["failed"].isBool();
  objectives = data["objectives"].toVariant().toMap();
  script = new ScriptController(SCRIPTS_PATH + "/quests/" + name + ".mjs");
  script->initialize(this);
}

QJsonObject Quest::save() const
{
  QJsonObject data;

  data.insert("name", name);
  data.insert("over", completed);
  data.insert("location", location);
  if (hidden)
    data.insert("hidden", hidden);
  if (failed)
    data.insert("failed", true);
  data.insert("objectives", QJsonValue::fromVariant(objectives));
  StorableObject::save(data);
  return data;
}

void Quest::setHidden(bool value)
{
  if (hidden && !value)
    playNewQuestSound();
  hidden = value;
  emit completedChanged();
}

void Quest::addObjective(const QString& name, const QString& label)
{
  if (objectives.count(name))
    objectives[name].toMap()["label"] = label;
  else
    objectives[name] = QVariantMap{{"label",label}};
}

void Quest::completeObjective(const QString& name)
{
  if (!isObjectiveCompleted(name))
  {
    if (objectives.count(name))
      objectives[name].toMap()["success"] = true;
    else
      objectives[name] = QVariantMap{{"success",true}};
    if (script && script->hasMethod("completeObjective"))
      script->call("completeObjective", QJSValueList() << name << true);
  }
}

void Quest::failObjective(const QString& name)
{
  if (objectives.count(name))
    objectives[name].toMap()["failure"] = true;
  else
    objectives[name] = QVariantMap{{"failure", true}};
  if (script && script->hasMethod("completeObjective"))
    script->call("completeObjective", QJSValueList() << name << false);
}

bool Quest::isObjectiveCompleted(const QString& name) const
{
  auto objective = objectives.find(name);
  QVariantMap params;

  if (script && script->hasMethod("isObjectiveCompleted"))
    return script->call("isObjectiveCompleted", QJSValueList() << name).toBool();
  if (objective != objectives.end())
    params = objective->toMap();
  return params.contains("success") && params["success"] == true;
}

bool Quest::areObjectivesCompleted(const QStringList& names) const
{
  for (const QString& name : names)
  {
    if (!isObjectiveCompleted(name))
      return false;
  }
  return true;
}

void Quest::onCharacterKilled(Character* character, Character* killer)
{
  if (!completed && script->hasMethod("onCharacterKilled"))
  {
    QJSValueList args;

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

void Quest::onLevelChanged()
{
  if (!completed && script->hasMethod("onLevelChanged"))
    script->call("onLevelChanged");
}

QVariantList Quest::getObjectives() const
{
  if (script->hasMethod("getObjectives"))
    return script->call("getObjectives").toVariant().toList();
  else
    qDebug() << "Quest" << name << ": missing `getObjectives` method";
  return objectives.values();
}

QJSValue Quest::getScriptObject() const
{
  if (script)
    return script->getObject();
  return QJSValue();
}

QString Quest::getDescription() const
{
  if (script && script->hasMethod("getDescription"))
    return script->call("getDescription").toString();
  return translate("description");
}

QString Quest::translate(const QString& key, const QVariantMap& options) const
{
  return I18n::get()->t("quests." + name + '.' + key, options);
}

void Quest::onCompletedChanged()
{
  if (completed && script && script->hasMethod("onCompleted"))
  {
    script->call("onCompleted");
    Game::get()->getSoundManager()->play("pipbuck/questdone");
  }
}

int Quest::getObjectiveCount() const
{
  return getObjectives().size();
}

int Quest::getCompleteCount() const
{
  int count = 0;

  for (auto it : getObjectives())
  {
    if (it.toMap()["success"].toBool())
      ++count;
  }
  return count;
}
