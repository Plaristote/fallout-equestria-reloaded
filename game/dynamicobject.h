#ifndef  DYNAMICOBJECT_H
# define DYNAMICOBJECT_H

# include "globals.h"
# include <QObject>
# include <QPoint>
# include <QJSValue>
# include <QTimer>
# include <QJsonObject>
# include "objects/components/lightsource.h"
# include "taskrunner.h"

class TileZone;
class Character;
class ObjectGroup;

class DynamicObject : public LightSourceComponent
{
  Q_OBJECT
  typedef LightSourceComponent ParentType;

  Q_PROPERTY(QString objectName MEMBER objectName NOTIFY objectNameChanged)
  Q_PROPERTY(QString path READ getPath NOTIFY pathChanged)
  Q_PROPERTY(TaskRunner* tasks MEMBER taskManager)
  Q_PROPERTY(bool blocksPath MEMBER blocksPath NOTIFY blocksPathChanged)
  Q_PROPERTY(bool isVisible READ isVisible NOTIFY visibilityChanged)
  Q_PROPERTY(QPoint spriteOffset READ getSpriteOffset WRITE setSpriteOffset NOTIFY spritePositionChanged)
  Q_PROPERTY(ObjectGroup* parent READ getGroup NOTIFY parentChanged)

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
  ObjectGroup* getGroup() const;
  QString getPath() const;
  const QString& getObjectName() const { return objectName; }
  QString getBaseName() const;
  TaskRunner* getTaskManager() { return taskManager; }

  Q_INVOKABLE QString getObjectType() const { return metaObject()->className(); }
  Q_INVOKABLE virtual int getZIndex() const { return 1; }
  Q_INVOKABLE virtual bool hasInteractionOverlay() const { return true; }
  virtual int getInteractionDistance() const { return 1; }
  virtual QStringList getAvailableInteractions();
  void setPosition(QPoint value) { position = value; emit positionChanged(); }
  virtual int getCoverValue() const { return 100; }
  QPoint getSpriteOffset() const;
  void   setSpriteOffset(QPoint);

  Q_INVOKABLE virtual bool triggerInteraction(Character*, const QString& interactionType);
  Q_INVOKABLE virtual bool triggerSkillUse(Character* user, const QString& skillName);

signals:
  void objectNameChanged();
  void blocksPathChanged();
  void visibilityChanged();
  void parentChanged();
  void pathChanged();

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
  QPoint nextPosition;
};

#endif // DYNAMICOBJECT_H
