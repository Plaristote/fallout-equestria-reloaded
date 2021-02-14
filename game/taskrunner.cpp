#include "globals.h"
#include "taskrunner.h"
#include "game.h"
#include <QJsonArray>

TaskRunner::TaskRunner(QObject *parent) : QObject(parent)
{

}

void TaskRunner::update(qint64 delta)
{
  for (auto it = tasks.begin() ; it != tasks.end() ; )
  {
    auto elapsedTime = delta;

    while (it->iterationCount > 0 && elapsedTime >= it->timeLeft)
    {
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
      if (it->iterationCount == 0)
        it = tasks.erase(it);
      else
        it++;
    }
  }
}

bool TaskRunner::runTask(Task& task)
{
  QJSValueList args;
  auto& scriptEngine = Game::get()->getScriptEngine();

  args << scriptEngine.newQObject(parent());
  return task.callback.call(args).toBool();
}

void TaskRunner::addTask(const QString &name, QJsonObject &data, qint64 interval, int iterationCount)
{
  Task task;
  QString scriptName = name + ".mjs";

  task.type = ModularTask;
  task.name = name;
  task.interval = interval;
  if (iterationCount < 1)
  {
    task.infinite = true;
    task.iterationCount = 1;
  }
  else
    task.iterationCount = iterationCount;
  task.module = Game::get()->loadScript(SCRIPTS_PATH + "tasks/" + data["script"].toString(scriptName));
  task.callback = task.module.property("onTriggered");
  tasks << task;
}

void TaskRunner::addLocalTask(const QString &name, QJsonObject &data, qint64 interval, int iterationCount)
{
  Task task;

  task.type = LocalTask;
  task.name = name;
  task.interval = interval;
  if (iterationCount < 1)
  {
    task.infinite = true;
    task.iterationCount = 1;
  }
  else
    task.iterationCount = iterationCount;
  task.module = module;
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
    task.data = taskData["data"].toObject();
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
    taskData["data"] = task.data;
    taskData["timeLeft"] = task.timeLeft;
    array << taskData;
  }
  data["tasks"] = array;
}
