#include "scriptable.h"
#include "game.h"

ScriptableComponent::ScriptableComponent(QObject *parent) : ParentType(parent)
{

}

ScriptableComponent::~ScriptableComponent()
{
  if (script)
    delete script;
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
  scriptName = data["script"].toString();
  ParentType::load(data);
  setScript(scriptName);
}

void ScriptableComponent::save(QJsonObject& data) const
{
  data["script"] = scriptName;
  ParentType::save(data);

}
