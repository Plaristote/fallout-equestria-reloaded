#ifndef  TASKRUNNER_H
# define TASKRUNNER_H

# include <QObject>
# include <QJSValue>
# include <QJsonObject>
# include "scriptcontroller.h"

class TaskRunner : public QObject
{
  Q_OBJECT

  struct Task
  {
    QString     name;
    int         iterationCount = 1;
    bool        infinite = false;
    qint64      interval;
    qint64      timeLeft;
  };

public:
  explicit TaskRunner(QObject *parent = nullptr);

  void update(qint64);

  void setScriptController(ScriptController* v) { script = v; }
  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE bool hasTask(const QString& name);
  Q_INVOKABLE void addTask(const QString& name, qint64 interval, int iterationCount = 1);
  Q_INVOKABLE bool removeTask(const QString& name);
  Q_INVOKABLE void decreaseIterationsFor(const QString& name, int iteractionCount);

signals:

private:
  bool runTask(Task&, int iterations);

  QList<Task> tasks;
  ScriptController* script = nullptr;
};

#endif // TASKRUNNER_H
