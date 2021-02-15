#include "globals.h"
#include "taskrunner.h"
#include "game.h"
#include <QJsonArray>

TaskRunner::TaskRunner(QObject *parent) : QObject(parent)
{

}

void TaskRunner::update(qint64 delta)
{
  for (auto it = tasks.begin() ; it != tasks.end() ;)
  {
    auto elapsedTime = delta;

    do {
      if (it->timeLeft <= elapsedTime)
      {
        it->timeLeft = it->interval;
        if (!it->infinite)
          it->iterationCount -= 1;
        if (!runTask(*it))
        {
          it->iterationCount = 0;
          break ;
        }
      }
      else
      {
        it->timeLeft -= elapsedTime;
        break ;
      }
    } while ((it->iterationCount > 0 || it->infinite) && elapsedTime >= it->timeLeft);
    if (it->iterationCount == 0)
      it = tasks.erase(it);
    else
      it++;
  }
}

bool TaskRunner::runTask(Task& task)
{
  QJSValueList args;
  auto& scriptEngine = Game::get()->getScriptEngine();

  args << scriptEngine.newQObject(parent());
  return Game::get()->scriptCall(task.callback, args, "TaskRunner").toBool();
}

void TaskRunner::addTask(const QString &name, qint64 interval, int iterationCount)
{
  Task task;
  QString scriptName = name + ".mjs";

  task.type = ModularTask;
  task.name = name;
  task.interval = interval;
  task.timeLeft = interval;
  if (iterationCount < 1)
  {
    task.infinite = true;
    task.iterationCount = 1;
  }
  else
    task.iterationCount = iterationCount;
  task.module = Game::get()->loadScript(SCRIPTS_PATH + "tasks/" + scriptName);
  task.callback = task.module.property("onTriggered");
  tasks << task;
}

void TaskRunner::addLocalTask(const QString &name, qint64 interval, int iterationCount)
{
  Task task;

  task.type = LocalTask;
  task.name = name;
  task.interval = interval;
  task.timeLeft = interval;
  if (iterationCount < 1)
  {
    task.infinite = true;
    task.iterationCount = 1;
  }
  else
    task.iterationCount = iterationCount;
  task.module = module;
  task.callback = task.module.property(name);
  tasks << task;
}

void TaskRunner::load(const QJsonObject& data)
{
  for (auto jvalue : data["tasks"].toArray())
  {
    QJsonObject taskData(jvalue.toObject());
    Task task;
    QString scriptName;

    task.type = taskData["type"].toInt(0) == 0 ? LocalTask : ModularTask;
    task.name = taskData["name"].toString();
    task.interval = taskData["interval"].toInt();
    task.infinite = taskData["infinite"].toBool();
    task.timeLeft = taskData["timeLeft"].toInt();
    scriptName = task.name + ".mjs";
    if (task.type == LocalTask)
    {
      task.module = Game::get()->loadScript(SCRIPTS_PATH + "tasks/" + data["script"].toString(scriptName));
      task.callback = task.module.property("onTriggered");
    }
    else
    {
      task.module = module;
      task.callback = task.module.property(data["callback"].toString(task.name));
    }
    tasks << task;
  }
}

void TaskRunner::save(QJsonObject& data) const
{
  QJsonArray array;

  for (const auto& task : tasks)
  {
    QJsonObject taskData;

    taskData["type"] = task.type == LocalTask ? 0 : 1;
    taskData["name"] = task.name;
    taskData["iteractionCount"] = task.iterationCount;
    taskData["infinite"] = task.infinite;
    taskData["interval"] = task.interval;
    taskData["timeLeft"] = task.timeLeft;
    array << taskData;
  }
  data["tasks"] = array;
}
