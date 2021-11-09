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
  if (callback.isCallable())
  {
    QJSValue retval = callback.call();

    return retval.isBool() ? retval.toBool() : true;
  }
  else
    qDebug() << character << ": ScriptAction callback is not callable.";
  return false;
}
