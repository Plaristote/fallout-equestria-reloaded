#include "scriptcontroller.h"
#include "game.h"

ScriptController::ScriptController(const QString& modulePath, QObject* object) :
  engine(Game::get()->getScriptEngine()), path(modulePath)
{
  QJSValue createCallback;
  Game*    game = Game::get();

  module = game->loadScript(path);
  createCallback = module.property("create");
  if (createCallback.isCallable())
    instance = game->scriptCall(createCallback, QJSValueList() << engine.newQObject(object), path);
  else
    qDebug() << "ScriptController: Missing `create` export in " << path;
}

bool ScriptController::hasMethod(const QString &method)
{
  QJSValue callback = instance.property(method);

  return callback.isCallable();
}

QJSValue ScriptController::call(const QString& method, const QJSValueList& args)
{
  QJSValue callback = instance.property(method);

  if (callback.isCallable())
  {
    QJSValue retval = callback.callWithInstance(instance, args);

    if (retval.isError())
    {
      qDebug() << path << ": uncaught exception at line "
               << retval.property("lineNumber").toInt() << ":" << retval.toString();
    }
    else
      return retval;
  }
  else
    qDebug() << "ScriptController: Missing method" << method << "in" << path;
  return false;
}

QJSValue ScriptController::property(const QString &name)
{
  return instance.property(name);
}
