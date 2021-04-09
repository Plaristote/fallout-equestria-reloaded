#ifndef  BUFF_H
# define BUFF_H

# include "utils/storableobject.h"
# include "game/scriptcontroller.h"
# include "game/taskrunner.h"
# include "globals.h"

class Character;

class Buff : public StorableObject
{
  Q_OBJECT

  Q_PROPERTY(QString     name   MEMBER name)
  Q_PROPERTY(Character*  target MEMBER target)
  Q_PROPERTY(TaskRunner* tasks  MEMBER tasks)
public:
  explicit Buff(Character* parent = nullptr);

  void initialize(const QString& name);
  void addNewCharge();
  void load(const QJsonObject&);
  void save(QJsonObject&) const;
  void update(qint64);

  Q_INVOKABLE void remove();

  inline const QString& getName() const { return name; }

signals:
  void finish(Buff*);

private slots:
  void finalize();

private:
  void    loadScript();
  QString getScriptPath() const { return SCRIPTS_PATH + "buffs"; }

  QString           name;
  Character*        target = nullptr;
  ScriptController* script = nullptr;
  TaskRunner*       tasks;
};

#endif // BUFF_H
