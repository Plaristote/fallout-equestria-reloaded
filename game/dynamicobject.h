#ifndef  DYNAMICOBJECT_H
# define DYNAMICOBJECT_H

# include <QObject>
# include <QPoint>
# include <QJSValue>
# include "sprite.h"

class DynamicObject : public Sprite
{
  Q_OBJECT

  Q_PROPERTY(QPoint position     READ getPosition)
  Q_PROPERTY(QString currentZone READ getCurrentZone)
public:
  explicit DynamicObject(QObject *parent = nullptr);

  Q_INVOKABLE QPoint getPosition() const { return position; }
  void setPosition(QPoint value) { position = value; }
  void moveTo(int x, int y, QPoint renderPosition);

  const QList<QPoint>& getCurrentPath() const { return currentPath; }
  QList<QPoint>& rcurrentPath() { return currentPath; }

  const QString& getCurrentZone() const { return currentZone; }
  void setCurrentZone(const QString& value) { currentZone = value; }

signals:
  void reachedDestination();
  void pathBlocked();

private slots:
  void onMovementEnded();

protected:
  QJSValue script;
private:
  QPoint position, nextPosition;
  QList<QPoint> currentPath;
  QString currentZone;
};

#endif // DYNAMICOBJECT_H
