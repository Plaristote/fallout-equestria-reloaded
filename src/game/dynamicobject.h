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
  Q_PROPERTY(QString displayName READ getDisplayName NOTIFY objectNameChanged)
  Q_PROPERTY(QString path READ getPath NOTIFY pathChanged)
  Q_PROPERTY(TaskRunner* tasks MEMBER taskManager)
  Q_PROPERTY(bool blocksPath MEMBER blocksPath NOTIFY blocksPathChanged)
  Q_PROPERTY(bool isVisible READ isVisible NOTIFY visibilityChanged)
  Q_PROPERTY(bool destructible  READ isDestructible)
  Q_PROPERTY(bool interactive MEMBER interactive NOTIFY interactiveChanged)
  Q_PROPERTY(QPoint spriteOffset READ getSpriteOffset WRITE setSpriteOffset NOTIFY spritePositionChanged)
  Q_PROPERTY(ObjectGroup* parent READ getGroup NOTIFY parentChanged)
  Q_PROPERTY(QString type READ getObjectType)
  Q_PROPERTY(int zIndex READ getZIndex NOTIFY zIndexChanged)

public:
  explicit DynamicObject(QObject *parent = nullptr);
  virtual ~DynamicObject();

  virtual void update(qint64 v) { ParentType::update(v); }
  virtual void updateTasks(qint64 v);
  virtual void load(const QJsonObject&) override;
  virtual void save(QJsonObject&) const override;
  virtual void setScript(const QString& name) override;

  inline bool isCharacter() const { return getObjectType() == "Character"; }
  inline bool isDoorway() const { return getObjectType() == "Doorway"; }
  virtual bool isBlockingPath() const { return blocksPath; }
  inline bool isVisible() const { return visible && !isHidden(); }
  void setVisible(bool value);
  virtual bool isDestructible() const;

  void setObjectName(const QString& value) { objectName = value; emit objectNameChanged(); }
  ObjectGroup* getGroup() const;
  QString getPath() const;
  QString getRelativePath(const ObjectGroup&) const;
  const QString& getObjectName() const { return objectName; }
  QString getBaseName() const;
  virtual QString getDisplayName() const;
  TaskRunner* getTaskManager() { return taskManager; }

  Q_INVOKABLE QString getObjectType() const { return metaObject()->className(); }
  Q_INVOKABLE virtual int getZIndex() const { return 1; }
  Q_INVOKABLE virtual bool hasInteractionOverlay() const { return interactive; }
  Q_INVOKABLE QString getHint() const;
  virtual int getInteractionDistance() const { return blocksPath ? 1 : 0; }
  virtual QStringList getAvailableInteractions();
  QPoint getSpriteOffset() const;
  void   setSpriteOffset(QPoint);

  Q_INVOKABLE virtual bool triggerInteraction(Character*, const QString& interactionType);
  Q_INVOKABLE virtual bool triggerSkillUse(Character* user, const QString& skillName, std::function<bool()> fallback = std::function<bool()>());
  Q_INVOKABLE void playSound(const QString&, qreal volume = 1.f) const;

  void deleteLater() { emit beforeDestroy(this); QObject::deleteLater(); }

signals:
  void beforeDestroy(DynamicObject*);
  void objectNameChanged();
  void blocksPathChanged();
  void visibilityChanged();
  void parentChanged();
  void pathChanged();
  void coverChanged();
  void interactiveChanged();
  void zIndexChanged();

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
  bool interactive = true;
};

#endif // DYNAMICOBJECT_H
