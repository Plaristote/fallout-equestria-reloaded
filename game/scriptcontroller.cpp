#include "scriptcontroller.h"
#include "game.h"
#include <QDebug>

ScriptController::ScriptController(const QString& modulePath) :
  engine(Game::get()->getScriptEngine()), path(modulePath)
{
  Game* game = Game::get();

  module = game->loadScript(path);
}

void ScriptController::initialize(QObject* object)
{
  Game*     game = Game::get();
  QJSValue createCallback;

  createCallback = module.property("create");
  model = engine.newQObject(object);
  if (createCallback.isCallable())
    instance = game->scriptCall(createCallback, QJSValueList() << model, path);
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
