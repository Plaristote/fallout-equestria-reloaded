#include "quest.h"
#include <QJsonObject>
#include <QDebug>
#include "game.h"
#include "i18n.h"

Quest::Quest(QObject *parent) : StorableObject(parent)
{
  connect(this, &Quest::completedChanged, this, &Quest::onCompletedChanged);
}

Quest::~Quest()
{
  if (script)
    delete script;
}

void Quest::initialize(const QString& name)
{
  this->name = name;
  completed = failed = false;
  script = new ScriptController(SCRIPTS_PATH + "quests/" + name + ".mjs");
  script->initialize(this);
  if (script->hasMethod("initialize"))
    script->call("initialize");
}

void Quest::load(const QJsonObject& data)
{
  StorableObject::load(data);
  name = data["name"].toString();
  completed = data["over"].toBool(false);
  failed = data["failed"].isBool();
  script = new ScriptController(SCRIPTS_PATH + "/quests/" + name + ".mjs");
  script->initialize(this);
}

QJsonObject Quest::save() const
{
  QJsonObject data;

  data.insert("name", name);
  data.insert("over", completed);
  if (failed)
    data.insert("failed", true);
  StorableObject::save(data);
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
  return tr("description");
}

QString Quest::tr(const QString& key, const QVariantMap& options)
{
  return I18n::get()->t("quests." + name + '.' + key, options);
}

void Quest::onCompletedChanged()
{
  if (completed && script && script->hasMethod("onCompleted"))
    script->call("onCompleted");
}
