#ifndef CHARACTERMOVEMENT_H
#define CHARACTERMOVEMENT_H

#include "../objects/storageobject.h"
#define CHARACTER_BASE_OBJECT StorageObject

class CharacterMovement : public CHARACTER_BASE_OBJECT
{
  Q_OBJECT

  Q_PROPERTY(QString orientation READ getOrientation)
  Q_PROPERTY(QString movementMode READ getMovementMode WRITE setMovementMode)
public:
  explicit CharacterMovement(QObject *parent = nullptr);

  virtual void load(const QJsonObject&);
  virtual void save(QJsonObject&) const;

  void                      setAnimation(const QString& animationName) override;
  void                      moveTo(int x, int y);
  Q_INVOKABLE void          lookTo(int x, int y);
  inline void               lookTo(QPoint target) { lookTo(target.x(), target.y()); }
  const QString&            getMovementMode() const { return movementMode; }
  void                      setMovementMode(const QString&);
  const QString&            getOrientation() const { return orientation; }
  const QList<QPoint>&      getCurrentPath() const { return currentPath; }
  const QVector<TileZone*>& getCurrentZones() const { return currentZones; }
  QList<QPoint>&            rcurrentPath() { return currentPath; }
  Q_INVOKABLE bool          isInZone(TileZone* value) const { return currentZones.indexOf(value) >= 0; }

public slots:
  void onIdle();
  void onZoneEntered(TileZone* value) { currentZones.append(value); }
  void onZoneExited(TileZone* value) { currentZones.removeOne(value); }

signals:
  void reachedDestination();
  void pathBlocked();

private slots:
  void onMovementEnded();
  void onDestinationReached();

protected:
  QList<QPoint>      currentPath;
  QString            orientation;
  QString            movementMode;
  QVector<TileZone*> currentZones;
};

#endif // CHARACTERMOVEMENT_H
