#ifndef  DYNAMICOBJECT_H
# define DYNAMICOBJECT_H

# include "globals.h"
# include <QObject>
# include <QPoint>
# include <QJSValue>
# include <QTimer>
# include <QJsonObject>
# include "objects/components/controlzone.h"
# include "taskrunner.h"

class TileZone;
class Character;

class DynamicObject : public ControlZoneComponent
{
  Q_OBJECT
  typedef ControlZoneComponent ParentType;

  Q_PROPERTY(QString objectName MEMBER objectName NOTIFY objectNameChanged)
  Q_PROPERTY(QPoint  position    READ getPosition NOTIFY positionChanged)
  Q_PROPERTY(QString currentZone READ getCurrentZone)
  Q_PROPERTY(TaskRunner* tasks MEMBER taskManager)
  Q_PROPERTY(bool blocksPath MEMBER blocksPath NOTIFY blocksPathChanged)
  Q_PROPERTY(bool isVisible MEMBER visible NOTIFY visibilityChanged)

public:
  explicit DynamicObject(QObject *parent = nullptr);
  virtual ~DynamicObject();

  virtual void update(qint64 v) { ParentType::update(v); }
  virtual void updateTasks(qint64 v);
  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;
  virtual void setScript(const QString& name);

  inline bool isCharacter() const { return getObjectType() == "Character"; }
  virtual bool isBlockingPath() const { return true; }
  inline bool isVisible() const { return visible; }
  void setVisible(bool value);

  void setObjectName(const QString& value) { objectName = value; emit objectNameChanged(); }
  const QString& getObjectName() const { return objectName; }
  TaskRunner* getTaskManager() { return taskManager; }

  Q_INVOKABLE QString getObjectType() const { return metaObject()->className(); }
  Q_INVOKABLE QPoint getPosition() const { return position; }
  Q_INVOKABLE virtual int getZIndex() const { return 1; }
  Q_INVOKABLE virtual QPoint getInteractionPosition() const { return interactionPosition; }
  virtual QStringList getAvailableInteractions();
  void setPosition(QPoint value) { position = value; emit positionChanged(); }
  Q_INVOKABLE void setInteractionPosition(int x, int y) { setInteractionPosition(QPoint(x, y)); }
  void setInteractionPosition(QPoint value) { interactionPosition = value; }
  virtual int getCoverValue() const { return 100; }

  Q_INVOKABLE virtual bool triggerInteraction(Character*, const QString& interactionType);
  Q_INVOKABLE virtual bool triggerSkillUse(Character* user, const QString& skillName);

  const QString& getCurrentZone() const { return currentZone; }
  void setCurrentZone(const QString& value) { currentZone = value; }

signals:
  void objectNameChanged();
  void blocksPathChanged();
  void positionChanged();
  void visibilityChanged();

protected slots:
  void onBlocksPathChanged();

protected:
  TaskRunner* taskManager;
  bool blocksPath = true;
//private:
  QString objectName;
  bool visible = true;
  QPoint position, nextPosition;
  QString currentZone;
  QPoint interactionPosition;
  QString orientation;
};

#endif // DYNAMICOBJECT_H
