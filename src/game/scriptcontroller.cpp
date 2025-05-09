#include "scriptcontroller.h"
#include "game.h"
#include <QDebug>

#ifdef GAME_EDITOR
static bool shouldLoadScript(const QString& path)
{
  return !Game::get()->getIsGameEditor()
      || path.startsWith(SCRIPTS_PATH + "items/");
}
#endif

ScriptController::ScriptController(const QString& modulePath) :
  engine(Game::get()->getScriptEngine()), path(modulePath)
{
#ifdef GAME_EDITOR
  if (shouldLoadScript(path))
#endif
  {
    module = Game::get()->loadScript(path);
  }
}

static int findLastPos(const QString& path, char c)
{
  QString name;
  int lastSlash = -1;

  for (int i = 0 ; i < path.size() ; ++i)
  {
    if (path[i] == '/')
      lastSlash = i;
  }
  return lastSlash;
}

static QString lastPart(const QString& path)
{
  QString name;
  int lastSlash = findLastPos(path, '/');

  for (int i = lastSlash + 1 ; i < path.size() ; ++i)
    name += path[i];
  return name;
}

static QString pathToClassName(const QString& path)
{
  QRegularExpression regex("\\.m?js$");
  QRegularExpression separator("[-_.]+");
  QString name = lastPart(path).replace(regex, "");
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
  QString objectName = pathToClassName(path);
  QJSValueList parameters;

  model = engine.newQObject(object);
  parameters << model;
  if (module.property("default").isCallable())
    instance = callConstructor(module.property("default"), parameters);
  else if (module.property(objectName).isCallable())
    instance = callConstructor(module.property(objectName), parameters);
  else if (module.property("create").isCallable())
    instance = callFunction(module.property("create"), parameters);
  else
    qDebug() << "ScriptController: Cannot find" << objectName << " in " << path;
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
  QString message = path + ": " + retval.toString();

  if (Game::get())
    Game::get()->javascriptError("uncaught exception at " + message);
  return head + message;
}

QJSValue ScriptController::call(const QString& method, const QJSValueList& args)
{
  QJSValue callback = instance.property(method);

  if (callback.isCallable())
    return callMethod(callback, args);
  else
    qDebug() << "ScriptController: Missing method" << method << "in" << path;
  return false;
}

QJSValue ScriptController::callMethod(QJSValue callback, const QJSValueList& args)
{
  QJSValue retval = callback.callWithInstance(instance, args);

  if (retval.isError())
  {
    qDebug() << jsErrorBacktrace(retval);
    return false;
  }
  return retval;
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
