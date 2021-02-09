#ifndef  TILEMAP_H
# define TILEMAP_H

# include <QObject>
# include <QSize>
# include <QQmlListProperty>
# include "tileset.h"
# include "tilelayer.h"
# include "tilezone.h"

class TileMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QSize tileSize MEMBER tileSize)
  Q_PROPERTY(QSize mapSize MEMBER mapSize)
  Q_PROPERTY(QQmlListProperty<TileZone> zones READ getZonesQml)
  Q_PROPERTY(QQmlListProperty<TileLayer> roofs READ getRoofsQml)
public:
  explicit TileMap(QObject *parent = nullptr);

  bool load(const QString& name);

  inline const QSize& getSize() const { return mapSize; }
  inline const QSize& getTileSize() const { return tileSize; }
  QList<TileZone*>& getZones() { return zones; }

  Q_INVOKABLE TileLayer* getLayer(const QString& name);
  Q_INVOKABLE TileZone*  getZone(const QString& name);

  QQmlListProperty<TileZone> getZonesQml() { return   QQmlListProperty<TileZone>(this, &zones); }
  QQmlListProperty<TileLayer> getRoofsQml() { return   QQmlListProperty<TileLayer>(this, &roofs); }

private:
  QSize               tileSize;
  QSize               mapSize;
  QVector<Tileset*>   tilesets;
  QVector<TileLayer*> layers;
  QList<TileZone*>    zones;
  QList<TileLayer*>   roofs;
};

#endif // LEVELMAP_H
