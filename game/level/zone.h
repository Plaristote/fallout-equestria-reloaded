#ifndef  ZONECOMPONENT_H
# define ZONECOMPONENT_H

# include "grid.h"

class ZoneComponent : public GridComponent
{
  Q_OBJECT
  typedef GridComponent ParentType;
public:
  ZoneComponent(QObject* parent = nullptr);

  void load(const QJsonObject&);

  virtual void registerDynamicObject(DynamicObject*) override;
  virtual void unregisterDynamicObject(DynamicObject*) override;
  void registerZoneController(ControlZoneComponent*) override;
  void unregisterZoneController(ControlZoneComponent*) override;

  Q_INVOKABLE bool isInsideZone(const TileZone*, const DynamicObject*) const;
  Q_INVOKABLE QJSValue getZoneOccupants(const TileZone* zone) const;

signals:
  void exitZoneEntered(TileZone*);

protected slots:
  void         registerZone(TileZone*);
  void         unregisterZone(TileZone*);
  void         onZoneEntered(DynamicObject*, TileZone*);
  void         onZoneExited(DynamicObject*, TileZone*);
  void         onZoneChangedFloor(TileZone*);
};

#endif // ZONECOMPONENT_H
