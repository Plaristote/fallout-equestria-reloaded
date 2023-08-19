#include "script.h"

QString jsErrorBacktrace(QJSValue retval);

ScriptAction::ScriptAction(Character* character, QJSValue config) : ActionBase(character)
{
  if (config.isCallable())
    triggerCallback = config;
  else if (config.isObject()) {
    triggerCallback = config.property("onTrigger");
    cancelCallback  = config.property("onCancel");
  }
}

void ScriptAction::update()
{
  state = Done;
}

bool ScriptAction::trigger()
{
  if (triggerCallback.isCallable())
    return call(triggerCallback);
  else if (!triggerCallback.isUndefined())
    qDebug() << character << ": ScriptAction callback is not callable.";
  return false;
}

void ScriptAction::canceled()
{
  if (cancelCallback.isCallable())
    call(cancelCallback);
}

bool ScriptAction::call(QJSValue callback)
{
  QJSValue retval = callback.call();

  if (retval.isError())
    jsErrorBacktrace(retval);
  return retval.isBool() ? retval.toBool() : true;
}
