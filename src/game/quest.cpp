#include "quest.h"
#include <QJsonObject>
#include <QDebug>
#include "game.h"
#include "i18n.h"

static void playNewQuestSound()
{
  Game::get()->getSoundManager()->play("pipbuck/newquest");
}

template<typename VALUE>
static void setPropertyOnObjective(QVariantMap& objectives, const QString& objectiveName, const QString& flagName, VALUE value)
{
  QVariantMap result;

  if (objectives.count(objectiveName))
  {
    result = objectives[objectiveName].toMap();
    result[flagName] = value;
  }
  else
    result = QVariantMap{{flagName,value}};
  objectives[objectiveName] = result;
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
  setPropertyOnObjective(objectives, name, "label", label);
}

bool Quest::hasObjective(const QString& name) const
{
  return objectives.find(name) != objectives.end();
}

void Quest::completeObjective(const QString& name)
{
  if (!isObjectiveCompleted(name))
  {
    setPropertyOnObjective(objectives, name, "success", true);
    if (script && script->hasMethod("completeObjective"))
      script->call("completeObjective", QJSValueList() << name << true);
  }
}

void Quest::failObjective(const QString& name)
{
  setPropertyOnObjective(objectives, name, "failed", true);
  if (script && script->hasMethod("completeObjective"))
    script->call("completeObjective", QJSValueList() << name << false);
}

bool Quest::getObjectiveFlag(const QString& name, const QString& flag, const QString& hookName) const
{
  QVariantMap::const_iterator objective;
  QVariantMap params;

  if (script && script->hasMethod(hookName))
  {
    QJSValue retval = script->call(hookName, QJSValueList() << name);

    if (retval.isBool())
      return retval.toBool();
  }
  objective = objectives.constFind(name);
  if (objective != objectives.end())
    params = objective->toMap();
  return params.contains(flag) && params[flag].toBool();
}

bool Quest::isObjectiveCompleted(const QString& name) const
{
  return getObjectiveFlag(name, "success", "isObjectiveCompleted");
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

bool Quest::isObjectiveFailed(const QString& name) const
{
  return getObjectiveFlag(name, "failed", "isObjectiveFailed");
}

QString Quest::objectiveNameAt(int index) const
{
  auto keys = objectives.keys();

  return keys.size() > index ? keys[index] : QString();
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
  qDebug() << "getRawObjectives:" << objectives.values();
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
