#include "globals.h"
#include "taskrunner.h"
#include "game.h"
#include <QJsonArray>
#include <cmath>

TaskRunner::TaskRunner(QObject *parent) : QObject(parent)
{

}

void TaskRunner::update(qint64 delta)
{
  if (delta > 1000)
    qDebug() << "TaskRunner::update starting";
  for (auto it = tasks.begin() ; it != tasks.end() ;)
  {
    auto elapsedTime = delta;

    if (elapsedTime > 1000)
      qDebug() << "TaskRunner::update: time left =" << elapsedTime;
    if (it->timeLeft <= elapsedTime)
    {
      int totalIterations = 1;

      elapsedTime -= it->timeLeft;
      if (elapsedTime >= it->interval)
      {
        totalIterations += static_cast<int>(elapsedTime / it->interval);
        it->timeLeft = elapsedTime % it->interval;
      }
      else
        it->timeLeft = it->interval - elapsedTime;
      if (!it->infinite && totalIterations >= it->iterationCount)
      {
        totalIterations = it->iterationCount;
        it->iterationCount = 0;
      }
      else if (!it->infinite)
        it->iterationCount -= totalIterations;
      if (!runTask(*it, totalIterations))
        it->iterationCount = 0;
    }
    else
      it->timeLeft -= elapsedTime;
    if (it->iterationCount == 0)
      it = tasks.erase(it);
    else
      it++;
  }
}

bool TaskRunner::runTask(Task& task, int iterations)
{
  if (script)
  {
    QJSValue retval;
    QJSValueList args;

    args << iterations;
    script->call(task.name, args);
    return retval.isBool() ? retval.toBool() : true;
  }
  else
    qDebug() << "TaskRunner::runTask: script controller uninitialized";
  return false;
}

bool TaskRunner::hasTask(const QString &name)
{
  for (auto it = tasks.begin() ; it != tasks.end() ; ++it)
  {
    if (it->name == name)
      return true;
  }
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

bool TaskRunner::removeTask(const QString &name)
{
  for (auto it = tasks.begin() ; it != tasks.end() ; ++it)
  {
    if (it->name == name)
    {
      tasks.erase(it);
      return true;
    }
  }
  return false;
}

void TaskRunner::decreaseIterationsFor(const QString &name, int iterationCount)
{
  for (auto it = tasks.begin() ; it != tasks.end() ;)
  {
    if (it->name == name)
    {
      if (it->iterationCount > iterationCount)
      {
        it->iterationCount -= iterationCount;
        it->timeLeft = it->interval;
        break ;
      }
      else if (it->iterationCount == iterationCount)
      {
        tasks.erase(it);
        break ;
      }
      else
      {
        iterationCount -= it->iterationCount;
        it = tasks.erase(it);
      }
    }
    else
      ++it;
  }
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
