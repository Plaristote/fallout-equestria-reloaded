#ifndef  DYNAMICOBJECT_H
# define DYNAMICOBJECT_H

# include <QObject>
# include <QPoint>
# include <QJSValue>
# include <QTimer>
# include <QJsonObject>
# include "sprite.h"

class DynamicObject : public Sprite
{
  Q_OBJECT

  Q_PROPERTY(QString objectName MEMBER objectName NOTIFY objectNameChanged)
  Q_PROPERTY(QPoint  position    READ getPosition)
  Q_PROPERTY(QString currentZone READ getCurrentZone)
  Q_PROPERTY(QJsonObject dataStore MEMBER dataStore)

public:
  explicit DynamicObject(QObject *parent = nullptr);

  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;

  bool isCharacter() const { return QString(metaObject()->className()) == "Character"; }

  void setObjectName(const QString& value) { objectName = value; emit objectNameChanged(); }
  const QString& getObjectName() const { return objectName; }
  void setScript(const QString& name);

  Q_INVOKABLE QPoint getPosition() const { return position; }
  Q_INVOKABLE virtual QPoint getInteractionPosition() const { return interactionPosition; }
  virtual QStringList getAvailableInteractions();
  void setPosition(QPoint value) { position = value; }
  void setInteractionPosition(QPoint value) { interactionPosition = value; }
  void moveTo(int x, int y, QPoint renderPosition);

  Q_INVOKABLE void setTickBehavior(int interval, bool repeat = false);

  const QList<QPoint>& getCurrentPath() const { return currentPath; }
  QList<QPoint>& rcurrentPath() { return currentPath; }

  const QString& getCurrentZone() const { return currentZone; }
  void setCurrentZone(const QString& value) { currentZone = value; }

signals:
  void objectNameChanged();
  void reachedDestination();
  void pathBlocked();

private slots:
  void onMovementEnded();
  void onDestinationReached();
  void onTicked();

protected:
  virtual QString getScriptPath() const { return ":/scripts/behaviour"; }
  QJSValue script;
private:
  QString objectName, scriptName;
  QPoint position, nextPosition;
  QTimer tick;
  QList<QPoint> currentPath;
  QString currentZone;
  QPoint interactionPosition;
  QJsonObject dataStore;
};

#endif // DYNAMICOBJECT_H
