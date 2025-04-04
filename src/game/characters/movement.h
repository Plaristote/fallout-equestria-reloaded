#ifndef CHARACTERMOVEMENT_H
#define CHARACTERMOVEMENT_H

#include "../objects/storageobject.h"
#include "utils/point.h"

class CharacterMovement : public StorageObject
{
  typedef StorageObject ParentType;
  Q_OBJECT

  Q_PROPERTY(QString movementMode READ getMovementMode WRITE setMovementMode)
public:
  explicit CharacterMovement(QObject *parent = nullptr);

  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;

  void                      moveTo(Point);
  Q_INVOKABLE void          lookAt(const DynamicObject* object) { if (object) { lookTo(object->getPosition()); } }
  Q_INVOKABLE void          lookTo(int x, int y);
  inline void               lookTo(QPoint target) { lookTo(target.x(), target.y()); }
  const QString&            getMovementMode() const { return movementMode; }
  void                      setMovementMode(const QString&);
  const QList<Point>&       getCurrentPath() const { return currentPath; }
  const QVector<TileZone*>& getCurrentZones() const { return currentZones; }
  void                      clearCurrentZones() { currentZones.clear(); }
  void                      setCurrentZones(QVector<TileZone*> value) { currentZones = value; }
  QList<Point>&             rcurrentPath() { return currentPath; }
  Q_INVOKABLE bool          isInZone(TileZone* value) const { return currentZones.indexOf(value) >= 0; }

public slots:
  virtual void onIdle();
  void onZoneEntered(TileZone* value) { currentZones.append(value); }
  void onZoneExited(TileZone* value) { currentZones.removeOne(value); }

signals:
  void reachedDestination();
  void pathBlocked();

private slots:
  void onMovementStart();
  void onMovementEnded();
  void onDestinationReached();

protected:
  QList<Point>       currentPath;
  QString            movementMode;
  QVector<TileZone*> currentZones;
};

#endif // CHARACTERMOVEMENT_H
