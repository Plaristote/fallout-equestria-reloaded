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
  QJSValue createCallback;

  createCallback = module.property("create");
  model = engine.newQObject(object);
  if (createCallback.isCallable())
    instance = callFunction(createCallback, QJSValueList() << model);
  else
    qDebug() << "ScriptController: Missing `create` export in " << path;
}

bool ScriptController::hasMethod(const QString &method)
{
  QJSValue callback = instance.property(method);

  return callback.isCallable();
}

QString jsErrorBacktrace(QJSValue retval)
{
  static QString head("ScriptController: uncaught exception at ");
  QString path  = retval.property("fileName").toString() + ':' + QString::number(retval.property("lineNumber").toInt());

  return head + path + ": " + retval.toString();
}

QJSValue ScriptController::call(const QString& method, const QJSValueList& args)
{
  QJSValue callback = instance.property(method);

  if (callback.isCallable())
  {
    QJSValue retval = callback.callWithInstance(instance, args);

    if (retval.isError())
      qDebug() << jsErrorBacktrace(retval);
    else
      return retval;
  }
  else
    qDebug() << "ScriptController: Missing method" << method << "in" << path;
  return false;
}

QJSValue ScriptController::callFunction(QJSValue function, const QJSValueList& args)
{
  QJSValue retval = function.call(args);

  if (retval.isError())
  {
    qDebug() << jsErrorBacktrace(retval);
    return false;
  }
  return retval;
}

QJSValue ScriptController::property(const QString &name)
{
  return instance.property(name);
}
