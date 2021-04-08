#include "scriptable.h"
#include "game.h"

ScriptableComponent::ScriptableComponent(QObject *parent) : ParentType(parent)
{
  if (!Game::get()->property("isGameEditor").toBool())
    connect(this, &ScriptableComponent::scriptNameChanged, this, &ScriptableComponent::initializeIfNeeded);
}

ScriptableComponent::~ScriptableComponent()
{
  if (script)
    delete script;
}

void ScriptableComponent::initializeIfNeeded()
{
  if (!scriptInitialized && script && script->hasMethod("initialize"))
  {
    script->call("initialize");
    scriptInitialized = true;
  }
}

void ScriptableComponent::setScript(const QString& name)
{
  if (script)
    delete script;
  scriptName = name;
  script     = new ScriptController(getScriptPath() + '/' + name);
  script->initialize(this);
  emit scriptNameChanged();
}

QJSValue ScriptableComponent::scriptCall(const QString& method, const QString& message)
{
  if (script)
    return script->call(method, QJSValueList() << message);
  return QJSValue();
}

QJSValue ScriptableComponent::getScriptObject() const
{
  if (script)
    return script->getObject();
  return QJSValue();
}

QJSValue ScriptableComponent::asJSValue()
{
  if (script)
    return script->getModel();
  return Game::get()->getScriptEngine().newQObject(this);
}

void ScriptableComponent::load(const QJsonObject& data)
{
  scriptInitialized = data["init"].toBool(false);
  scriptName = data["script"].toString();
  ParentType::load(data);
  setScript(scriptName);
}

void ScriptableComponent::save(QJsonObject& data) const
{
  if (scriptInitialized)
    data["init"] = scriptInitialized;
  data["script"] = scriptName;
  ParentType::save(data);

}
