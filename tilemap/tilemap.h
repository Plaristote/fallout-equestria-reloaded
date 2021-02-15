#ifndef  TILEMAP_H
# define TILEMAP_H

# include <QObject>
# include <QSize>
# include <QQmlListProperty>
# include "tileset.h"
# include "tilelayer.h"
# include "tilezone.h"
# include "objecttileset.h"
# include <QJsonObject>
# include <QStringList>


class TileMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QSize tileSize MEMBER tileSize)
  Q_PROPERTY(QSize mapSize MEMBER mapSize)
  Q_PROPERTY(QStringList textureList MEMBER textureList NOTIFY onTextureListChanged)
  Q_PROPERTY(QQmlListProperty<TileZone> zones READ getZonesQml)
  Q_PROPERTY(QQmlListProperty<TileLayer> roofs READ getRoofsQml)
public:
  explicit TileMap(QObject *parent = nullptr);

  bool load(const QString& name);

  inline const QSize& getSize() const { return mapSize; }
  inline int getPixelWidth() const { return (mapSize.width() - 1) * tileSize.width();}
  inline const QSize& getTileSize() const { return tileSize; }
  QList<TileZone*>& getZones() { return zones; }
  const QList<QJsonObject>& getObjects() const { return objects; }

  QString getObjectSource(int gid) const;
  QSize   getObjectSize(int gid) const;

  Q_INVOKABLE TileLayer* getLayer(const QString& name);
  Q_INVOKABLE TileZone*  getZone(const QString& name);

  void addTileZone(TileZone* zone)    { zones << zone; }
  void removeTileZone(TileZone* zone) { zones.removeAll(zone); }

  QQmlListProperty<TileZone> getZonesQml() { return   QQmlListProperty<TileZone>(this, &zones); }
  QQmlListProperty<TileLayer> getRoofsQml() { return   QQmlListProperty<TileLayer>(this, &roofs); }

signals:
  void onTextureListChanged();

private:
  QSize                   tileSize;
  QSize                   mapSize;
  QVector<Tileset*>       tilesets;
  QVector<TileLayer*>     layers;
  QVector<ObjectTileset*> objectTilesets;
  QList<TileZone*>        zones;
  QList<TileLayer*>       roofs;
  QList<QJsonObject>      objects;
  QStringList             textureList;
};

#endif // LEVELMAP_H
