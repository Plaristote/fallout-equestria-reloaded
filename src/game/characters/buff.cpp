#include "buff.h"
#include "game/character.h"

Buff::Buff(Character* parent) : StorableObject(parent), target(parent)
{
  tasks = new TaskRunner(this);
}

void Buff::initialize(const QString&)
{
  loadScript();
  if (script->hasMethod("initialize"))
    script->call("initialize");
}

void Buff::addNewCharge()
{
  if (script->hasMethod("repeat"))
    script->call("repeat");
}

void Buff::finalize()
{
  if (script->hasMethod("finalize"))
    script->call("finalize");
}

void Buff::loadScript()
{
  script = new ScriptController(getScriptPath() + '/' + name + ".mjs");
  script->initialize(this);
  tasks->setScriptController(script);
}

void Buff::load(const QJsonObject& data)
{
  name = data["name"].toString();
  StorableObject::load(data);
  loadScript();
  if (data["tasks"].isObject())
    tasks->load(data["tasks"].toObject());
}

void Buff::save(QJsonObject& data) const
{
  QJsonObject tasksData;

  tasks->save(tasksData);
  data["name"] = name;
  data["tasks"] = tasksData;
  StorableObject::save(data);
}

void Buff::update(qint64 delta)
{
  tasks->update(delta);
}

void Buff::remove()
{
  finalize();
  emit finish(this);
}

QJSValue Buff::getScriptObject() const
{
  if (script)
    return script->getObject();
  return QJSValue();
}
