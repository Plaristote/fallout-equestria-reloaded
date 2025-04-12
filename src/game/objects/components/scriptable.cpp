#include "scriptable.h"
#include "game.h"

ScriptableComponent::ScriptableComponent(QObject *parent) : ParentType(parent)
{
  if (!Game::get()->getIsGameEditor())
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
  if (name.length() > 0)
  {
    script = new ScriptController(getScriptPath() + '/' + name);
    script->initialize(this);
  }
  else
    script = nullptr;
  emit scriptNameChanged();
}

QJSValue ScriptableComponent::scriptProperty(const QString& name) const
{
  if (script)
    return script->property(name);
  return QJSValue();
}

QJSValue ScriptableComponent::scriptCall(const QString& method, const QJSValueList& params)  const
{
  if (script && script->hasMethod(method))
    return script->call(method, params);
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
