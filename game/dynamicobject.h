#ifndef  DYNAMICOBJECT_H
# define DYNAMICOBJECT_H

# include "globals.h"
# include <QObject>
# include <QPoint>
# include <QJSValue>
# include <QTimer>
# include <QJsonObject>
# include "sprite.h"
# include "taskrunner.h"
# include "scriptcontroller.h"

class TileZone;
class Character;

class DynamicObject : public Sprite
{
  Q_OBJECT

  Q_PROPERTY(QString objectName MEMBER objectName NOTIFY objectNameChanged)
  Q_PROPERTY(QString scriptName READ getScriptName NOTIFY scriptNameChanged)
  Q_PROPERTY(QPoint  position    READ getPosition)
  Q_PROPERTY(QString currentZone READ getCurrentZone)
  Q_PROPERTY(TaskRunner* tasks MEMBER taskManager)
  Q_PROPERTY(TileZone* controlZone MEMBER controlZone NOTIFY controlZoneChanged)
  Q_PROPERTY(bool blocksPath MEMBER blocksPath NOTIFY blocksPathChanged)

public:
  explicit DynamicObject(QObject *parent = nullptr);
  virtual ~DynamicObject();

  virtual void update(qint64 v) { Sprite::update(v); }
  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;
  Q_INVOKABLE virtual void setScript(const QString& name);

  inline bool isCharacter() const { return getObjectType() == "Character"; }
  virtual bool isBlockingPath() const { return true; }

  void setObjectName(const QString& value) { objectName = value; emit objectNameChanged(); }
  const QString& getObjectName() const { return objectName; }
  TaskRunner* getTaskManager() { return taskManager; }

  Q_INVOKABLE bool     hasVariable(const QString& name) const { return dataStore.contains(name); }
  Q_INVOKABLE QVariant getVariable(const QString& name) const { return dataStore[name].toVariant(); }
  Q_INVOKABLE void     setVariable(const QString& name, const QVariant& value) { dataStore.insert(name, QJsonValue::fromVariant(value)); }
  Q_INVOKABLE void     unsetVariable(const QString& name) { dataStore.remove(name); }

  Q_INVOKABLE QString getObjectType() const { return metaObject()->className(); }
  QString getScriptName() const { return scriptName; }
  Q_INVOKABLE QPoint getPosition() const { return position; }
  Q_INVOKABLE virtual int getZIndex() const { return 1; }
  Q_INVOKABLE virtual QPoint getInteractionPosition() const { return interactionPosition; }
  virtual QStringList getAvailableInteractions();
  void setPosition(QPoint value) { position = value; }
  Q_INVOKABLE void setInteractionPosition(int x, int y) { setInteractionPosition(QPoint(x, y)); }
  void setInteractionPosition(QPoint value) { interactionPosition = value; }
  virtual int getCoverValue() const { return 100; }

  Q_INVOKABLE TileZone* addControlZone();
  Q_INVOKABLE void      removeControlZone();
  TileZone*             getControlZone() { return controlZone; }

  Q_INVOKABLE virtual bool triggerInteraction(Character*, const QString& interactionType);
  Q_INVOKABLE virtual bool triggerSkillUse(Character* user, const QString& skillName);
  Q_INVOKABLE QJSValue  scriptCall(const QString& method, const QString& message = "");

  const QString& getCurrentZone() const { return currentZone; }
  void setCurrentZone(const QString& value) { currentZone = value; }

signals:
  void objectNameChanged();
  void scriptNameChanged();
  void blocksPathChanged();
  void controlZoneChanged();
  void controlZoneAdded(TileZone*);
  void controlZoneRemoved(TileZone*);

protected slots:
  void onBlocksPathChanged();

protected:
  virtual QString getScriptPath() const { return SCRIPTS_PATH + "behaviour"; }
  ScriptController* script = nullptr;
  TaskRunner* taskManager;
  TileZone* controlZone = nullptr;
  bool blocksPath = true;
//private:
  QString objectName, scriptName;
  QPoint position, nextPosition;
  QString currentZone;
  QPoint interactionPosition;
  QJsonObject dataStore;

  QString orientation;
};

#endif // DYNAMICOBJECT_H
