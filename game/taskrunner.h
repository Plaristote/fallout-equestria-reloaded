#ifndef  TASKRUNNER_H
# define TASKRUNNER_H

# include <QObject>
# include <QJSValue>
# include <QJsonObject>

class TaskRunner : public QObject
{
  Q_OBJECT

  enum TaskType
  {
    LocalTask = 0,
    ModularTask = 1
  };

  struct Task
  {
    TaskType    type;
    QString     name;
    int         iterationCount = 1;
    bool        infinite = false;
    QJsonObject data;
    QJSValue    callback, module;
    qint64      interval;
    qint64      timeLeft;
  };

public:
  explicit TaskRunner(QObject *parent = nullptr);

  void update(qint64);

  void setLocalModule(QJSValue v) { module = v; }
  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  void addTask(const QString& name, QJsonObject& data, qint64 interval, int iterationCount = 1);
  void addLocalTask(const QString& name, QJsonObject& data, qint64 interval, int iterationCount = 1);

signals:

private:
  bool runTask(Task&);

  QList<Task> tasks;
  QJSValue    module;
};

#endif // TASKRUNNER_H
