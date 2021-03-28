#ifndef  CONTROLZONECOMPONENT_H
# define CONTROLZONECOMPONENT_H

# include "scriptable.h"

class TileZone;

class ControlZoneComponent : public ScriptableComponent
{
  Q_OBJECT
  typedef ScriptableComponent ParentType;

  Q_PROPERTY(TileZone* controlZone MEMBER controlZone NOTIFY controlZoneChanged)
public:
  explicit ControlZoneComponent(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE TileZone* addControlZone();
  Q_INVOKABLE void      removeControlZone();
  TileZone*             getControlZone() { return controlZone; }

signals:
  void controlZoneChanged();
  void controlZoneAdded(TileZone*);
  void controlZoneRemoved(TileZone*);

protected:
  TileZone* controlZone = nullptr;
};

#endif // CONTROLZONECOMPONENT_H
