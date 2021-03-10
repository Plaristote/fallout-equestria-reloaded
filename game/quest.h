#ifndef QUEST_H
#define QUEST_H

#include <QObject>
#include "scriptcontroller.h"

class Character;
class InventoryItem;

class Quest : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString     name      MEMBER name      NOTIFY nameChanged)
  Q_PROPERTY(bool        completed MEMBER completed NOTIFY completedChanged)
  Q_PROPERTY(QVariantMap variables MEMBER variables)
public:
  enum ObjectiveState { InProgress = 0, Done, Failed };

  explicit Quest(QObject *parent = nullptr);
  ~Quest();

  void initialize(const QString& name);
  void load(const QJsonObject&);
  QJsonObject save() const;

  Q_INVOKABLE void completeObjective(const QString&);
  Q_INVOKABLE bool isObjectiveCompleted(const QString&) const;

  Q_INVOKABLE QVariantList getObjectives() const;

public slots:
  void onCharacterKilled(Character* victim, Character* killed);
  void onItemPicked(InventoryItem*);

signals:
  void nameChanged();
  void completedChanged();

private:
  QString           name;
  QVariantMap       variables;
  bool              completed;
  ScriptController* script = nullptr;
};

#endif // QUEST_H
