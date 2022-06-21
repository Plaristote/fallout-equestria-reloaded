#include "scriptcontroller.h"
#include "game.h"
#include <QDebug>

ScriptController::ScriptController(const QString& modulePath) :
  engine(Game::get()->getScriptEngine()), path(modulePath)
{
  Game* game = Game::get();

  module = game->loadScript(path);
}

static QString pathToClassName(const QString& path)
{
  QRegularExpression regex(".m?js$");
  QRegularExpression separator("[-_.]+");
  QString name = path.split("/").last().replace(regex, "");
  QString result;

  for (auto part : name.split(separator))
  {
    if (part.length() > 0)
    {
      part[0] = part.front().toUpper();
      result += part;
    }
  }
  return result;
}

void ScriptController::initialize(QObject* object)
{
  QJSValue createCallback;
  QString objectName = pathToClassName(path);
  QJSValueList parameters;

  model = engine.newQObject(object);
  parameters << model;
  createCallback = module.property(objectName);
  if (createCallback.isCallable())
    instance = callConstructor(createCallback, parameters);
  else
  {
    createCallback = module.property("create");
    if (createCallback.isCallable())
      instance = callFunction(createCallback, parameters);
    else
      qDebug() << "ScriptController: Cannot find" << objectName << " in " << path;
  }
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

QJSValue ScriptController::callConstructor(QJSValue constructor, const QJSValueList& args)
{
  QJSValue retval = constructor.callAsConstructor(args);

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
