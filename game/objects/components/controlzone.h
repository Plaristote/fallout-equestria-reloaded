#ifndef  CONTROLZONECOMPONENT_H
# define CONTROLZONECOMPONENT_H

# include "detectable.h"

class TileZone;
class DynamicObject;

class ControlZoneComponent : public DetectableComponent
{
  Q_OBJECT
  typedef DetectableComponent ParentType;

  Q_PROPERTY(TileZone* controlZone MEMBER controlZone NOTIFY controlZoneChanged)
  Q_PROPERTY(bool zoneBlocked MEMBER zoneBlocked NOTIFY zoneBlockedChanged)
public:
  explicit ControlZoneComponent(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE TileZone* addControlZone();
  Q_INVOKABLE void      removeControlZone();
  TileZone*             getControlZone() { return controlZone; }
  void                  toggleZoneBlocked(bool value);
  inline bool           hasControlZone() const { return controlZone != nullptr; }

signals:
  void controlZoneChanged();
  void controlZoneAdded(TileZone*);
  void controlZoneRemoved(TileZone*);
  void zoneBlockedChanged();

private slots:
  void listenControlZone(TileZone*);
  void stopListeningControlZone(TileZone*);
  void onZoneEntered(DynamicObject*, TileZone*);
  void onZoneExited(DynamicObject*, TileZone*);
  void updateZoneBlock();

private:
  TileZone* controlZone = nullptr;
  bool zoneBlocked = true;
};

#endif // CONTROLZONECOMPONENT_H
