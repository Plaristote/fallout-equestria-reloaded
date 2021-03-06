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
  Q_PROPERTY(TaskRunner* tasks MEMBER taskManager)
  Q_PROPERTY(bool blocksPath MEMBER blocksPath NOTIFY blocksPathChanged)
  Q_PROPERTY(bool isVisible READ isVisible NOTIFY visibilityChanged)

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
  inline bool isVisible() const { return visible && !isHidden(); }
  void setVisible(bool value);

  void setObjectName(const QString& value) { objectName = value; emit objectNameChanged(); }
  const QString& getObjectName() const { return objectName; }
  QString getBaseName() const;
  TaskRunner* getTaskManager() { return taskManager; }

  Q_INVOKABLE QString getObjectType() const { return metaObject()->className(); }
  Q_INVOKABLE QPoint getPosition() const { return position; }
  Q_INVOKABLE virtual int getZIndex() const { return 1; }
  Q_INVOKABLE virtual bool hasInteractionOverlay() const { return true; }
  virtual int getInteractionDistance() const { return 1; }
  virtual QStringList getAvailableInteractions();
  void setPosition(QPoint value) { position = value; emit positionChanged(); }
  virtual int getCoverValue() const { return 100; }

  Q_INVOKABLE virtual bool triggerInteraction(Character*, const QString& interactionType);
  Q_INVOKABLE virtual bool triggerSkillUse(Character* user, const QString& skillName);

signals:
  void objectNameChanged();
  void blocksPathChanged();
  //void positionChanged();
  void visibilityChanged();

protected slots:
  void onBlocksPathChanged();

private:
  virtual bool defaultLookInteraction();

protected:
  TaskRunner* taskManager;
  bool blocksPath = true;
//private:
  QString objectName;
  bool visible = true;
  QPoint position, nextPosition;
};

#endif // DYNAMICOBJECT_H
