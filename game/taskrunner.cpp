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
        elapsedTime -= it->timeLeft;
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
  if (script)
  {
    QJSValue retval;

    script->call(task.name);
    return retval.isBool() ? retval.toBool() : true;
  }
  else
    qDebug() << "TaskRunner::runTask: script controller uninitialized";
  return false;
}

void TaskRunner::addTask(const QString &name, qint64 interval, int iterationCount)
{
  Task task;

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
  tasks << task;
}

void TaskRunner::load(const QJsonObject& data)
{
  for (auto jvalue : data["tasks"].toArray())
  {
    QJsonObject taskData(jvalue.toObject());
    Task task;

    task.name           = taskData["name"].toString();
    task.iterationCount = taskData["count"].toInt();
    task.interval       = taskData["interval"].toInt();
    task.infinite       = taskData["infinite"].toBool();
    task.timeLeft       = taskData["timeLeft"].toInt();
    tasks << task;
  }
}

void TaskRunner::save(QJsonObject& data) const
{
  QJsonArray array;

  for (const auto& task : tasks)
  {
    QJsonObject taskData;

    taskData["name"]     = task.name;
    taskData["count"]    = task.iterationCount;
    taskData["infinite"] = task.infinite;
    taskData["interval"] = task.interval;
    taskData["timeLeft"] = task.timeLeft;
    array << taskData;
  }
  data["tasks"] = array;
}
