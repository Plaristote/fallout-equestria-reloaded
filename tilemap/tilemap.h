#ifndef  TILEMAP_H
# define TILEMAP_H

# include <QObject>
# include <QSize>
# include <QQmlListProperty>
# include "limits.h"
# include "tileset.h"
# include "tilelayer.h"
# include "tilezone.h"
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
  Q_PROPERTY(QQmlListProperty<TileLayer> lights READ getLightsQml)

  typedef void (TileMap::*LayerFolderLoader)(const QJsonObject&);
  static const QMap<QString, LayerFolderLoader> loaders;
public:
  explicit TileMap(QObject *parent = nullptr);

  bool load(const QString& name);

  inline const QSize& getSize() const { return mapSize; }
  inline int getPixelWidth() const { return (mapSize.width() - 1) * tileSize.width();}
  inline const QSize& getTileSize() const { return tileSize; }
  QList<TileZone*>& getZones() { return zones; }
  const QList<TileLayer*>& getRoofs() const { return roofs; }
  const QList<TileLayer*>& getLights() const { return lights; }
  Tileset* getTileset(const QString& name) const;

  Q_INVOKABLE TileLayer* getLayer(const QString& name);
  Q_INVOKABLE TileLayer* getRoofLayer(const QString& name);
  Q_INVOKABLE TileLayer* getLightLayer(const QString& name);
  Q_INVOKABLE TileZone*  getZone(const QString& name);

  QQmlListProperty<TileZone> getZonesQml() { return QQmlListProperty<TileZone>(this, &zones); }
  QQmlListProperty<TileLayer> getRoofsQml() { return QQmlListProperty<TileLayer>(this, &roofs); }
  QQmlListProperty<TileLayer> getLightsQml() { return QQmlListProperty<TileLayer>(this, &lights); }

public slots:
  void addTileZone(TileZone* zone)     { if (zones.indexOf(zone) < 0) { zones << zone; } }
  void removeTileZone(TileZone* zone)  { zones.removeAll(zone); }
  void addLightLayer(TileLayer* zone)    { if (lights.indexOf(zone) < 0) { lights << zone; } }
  void removeLightLayer(TileLayer* zone) { lights.removeAll(zone); }

signals:
  void onTextureListChanged();

private:
  void loadTilesets(const QJsonArray&);
  void loadLayers(const QJsonArray&);
  void loadRoofFolder(const QJsonObject&);
  void loadLightFolder(const QJsonObject&);
  void loadZoneFolder(const QJsonObject&);
  void loadLightTileset();

  QSize                   tileSize;
  QSize                   mapSize;
  QVector<Tileset*>       tilesets;
  QVector<TileLayer*>     layers;
  QList<TileZone*>        zones;
  QList<TileLayer*>       roofs;
  QList<TileLayer*>       lights;
  QStringList             textureList;
};

#endif // LEVELMAP_H
