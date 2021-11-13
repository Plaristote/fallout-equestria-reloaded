#include "script.h"

ScriptAction::ScriptAction(Character* character, QJSValue callback) : ActionBase(character), callback(callback)
{
}

void ScriptAction::update()
{
  state = Done;
}

bool ScriptAction::trigger()
{
  return call(true);
}

void ScriptAction::canceled()
{
  call(false);
}

bool ScriptAction::call(bool param)
{
  if (callback.isCallable())
  {
    QJSValue retval = callback.call(QJSValueList() << param);

    return retval.isBool() ? retval.toBool() : true;
  }
  else
    qDebug() << character << ": ScriptAction callback is not callable.";
  return false;
}
