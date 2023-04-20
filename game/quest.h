#ifndef QUEST_H
#define QUEST_H

#include <QObject>
#include "scriptcontroller.h"
#include "utils/storableobject.h"

class Character;
class InventoryItem;

class Quest : public StorableObject
{
  Q_OBJECT

  Q_PROPERTY(QString     name           MEMBER name            NOTIFY nameChanged)
  Q_PROPERTY(QString     location       MEMBER location        NOTIFY locationChanged)
  Q_PROPERTY(bool        hidden         READ isHidden WRITE setHidden NOTIFY completedChanged)
  Q_PROPERTY(bool        completed      MEMBER completed       NOTIFY completedChanged)
  Q_PROPERTY(bool        failed         MEMBER failed          NOTIFY completedChanged)
  Q_PROPERTY(bool        inProgress     READ inProgress        NOTIFY completedChanged)
  Q_PROPERTY(QString     description    READ getDescription    NOTIFY descriptionChanged)
  Q_PROPERTY(int         objectiveCount READ getObjectiveCount NOTIFY nameChanged)
  Q_PROPERTY(int         completeCount  READ getCompleteCount  NOTIFY completedChanged)
public:
  enum ObjectiveState { InProgress = 0, Done, Failed };

  explicit Quest(QObject *parent = nullptr);
  ~Quest() override;

  void initialize(const QString& name, bool hidden);
  void load(const QJsonObject&);
  QJsonObject save() const;

  Q_INVOKABLE void addObjective(const QString&, const QString&);
  Q_INVOKABLE void completeObjective(const QString&);
  Q_INVOKABLE bool isObjectiveCompleted(const QString&) const;
  inline bool inProgress() const { return !completed && !failed; }
  inline bool isHidden() const { return hidden; }
  void setHidden(bool value);

  Q_INVOKABLE QVariantList getObjectives() const;
  Q_INVOKABLE QJSValue getScriptObject() const;
  Q_INVOKABLE QString tr(const QString& key, const QVariantMap& options = {}) const { return translate(key, options); }
  QString translate(const QString& key, const QVariantMap& options = {}) const;
  QString getDescription() const;
  int getObjectiveCount() const;
  int getCompleteCount() const;

public slots:
  void onCharacterKilled(Character* victim, Character* killed);
  void onItemPicked(InventoryItem*);
  void onLevelChanged();

signals:
  void nameChanged();
  void completedChanged();
  void descriptionChanged();
  void locationChanged();

private slots:
  void onCompletedChanged();

private:
  QString           name, location;
  bool              completed, failed, hidden;
  ScriptController* script = nullptr;
  QVariantMap       objectives;
};

#endif // QUEST_H
