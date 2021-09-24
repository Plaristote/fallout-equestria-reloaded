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

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  Q_INVOKABLE QJSValue getZoneOccupants(TileZone* zone);

signals:
  void exitZoneEntered(TileZone*);

protected slots:
  void         registerZone(TileZone*);
  void         unregisterZone(TileZone*);
  void         onZoneEntered(DynamicObject*, TileZone*);
  void         onZoneExited(DynamicObject*, TileZone*);
};

#endif // ZONECOMPONENT_H
