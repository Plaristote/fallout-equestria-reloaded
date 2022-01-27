#ifndef  TILEMAP_H
# define TILEMAP_H

# include <QObject>
# include <QSize>
# include <QQmlListProperty>
# include "limits.h"
# include "tileset.h"
# include "tilelayer.h"
# include "tilezone.h"
# include "floorlayer.h"
# include <QJsonObject>
# include <QStringList>
# include "globals.h"

class TileMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QSize tileSize MEMBER tileSize CONSTANT)
  Q_PROPERTY(QSize mapSize MEMBER mapSize CONSTANT)
  Q_PROPERTY(QStringList textureList MEMBER textureList NOTIFY onTextureListChanged)
  Q_PROPERTY(QQmlListProperty<TileZone> zones READ getZonesQml NOTIFY zonesChanged)
  Q_PROPERTY(QQmlListProperty<TileLayer> roofs READ getRoofsQml CONSTANT)
  Q_PROPERTY(QQmlListProperty<TileLayer> lights READ getLightsQml NOTIFY lightsChanged)

  friend class FloorLayer;
  typedef void (TileMap::*LayerFolderLoader)(const QJsonObject&);
  static const QMap<QString, LayerFolderLoader> loaders;
public:
  explicit TileMap(QObject *parent = nullptr);

  bool load(const QString& name);
  void renderToFile(const QString& filename);
  void renderToImage(QImage& image, QPoint offset = {0,0});

  unsigned char getFloor() const { return floor; }
  inline const QSize& getSize() const { return mapSize; }
  inline int getPixelWidth() const { return (mapSize.width() - 1) * tileSize.width();}
  inline const QSize& getTileSize() const { return tileSize; }
  QList<TileZone*>& getZones() { return zones; }
  const QList<TileLayer*>& getRoofs() const { return roofs; }
  const QList<TileLayer*>& getLights() const { return lights; }
  const QVector<TileLayer*>& getLayers() const { return layers; }
  const QList<FloorLayer*>& getFloors() const { return floors; }
  const QList<TileZone*>& getPathfindindingZones() const { return pathfindindingZones; }
  Tileset* getTileset(const QString& name) const;
  Q_INVOKABLE QPoint getPointFor(int x, int y) const;

  Q_INVOKABLE TileLayer* getLayer(const QString& name);
  Q_INVOKABLE TileLayer* getRoofLayer(const QString& name);
  Q_INVOKABLE TileLayer* getLightLayer(const QString& name);
  Q_INVOKABLE TileZone*  getZone(const QString& name);
  Q_INVOKABLE TileMap*   getFloor(const QString& name);

  QQmlListProperty<TileZone> getZonesQml() { return QML_QLIST_CONSTRUCTOR(TileZone, zones); }
  QQmlListProperty<TileLayer> getRoofsQml() { return QML_QLIST_CONSTRUCTOR(TileLayer, roofs); }
  QQmlListProperty<TileLayer> getLightsQml() { return QML_QLIST_CONSTRUCTOR(TileLayer, lights); }

public slots:
  void addTileZone(TileZone* zone);
  void removeTileZone(TileZone* zone);
  void addLightLayer(TileLayer* zone);
  void removeLightLayer(TileLayer* zone);

signals:
  void onTextureListChanged();
  void zonesChanged();
  void lightsChanged();

private:
  void loadTilesets(const QJsonArray&);
  void loadLayers(const QJsonArray&);
  void loadRoofFolder(const QJsonObject&);
  void loadLightFolder(const QJsonObject&);
  void loadZoneFolder(const QJsonObject&);
  void loadWallFolder(const QJsonObject&);
  void loadFloorFolder(const QJsonObject&);
  void loadPathfinding(const QJsonObject&);
  void loadLightTileset();
  int getLastGid() const;

  QSize               tileSize;
  QSize               mapSize;
  QVector<Tileset*>   tilesets;
  QVector<TileLayer*> layers;
  QList<FloorLayer*>  floors;
  QList<TileZone*>    zones;
  QList<TileLayer*>   roofs;
  QList<TileLayer*>   lights;
  QList<TileZone*>    pathfindindingZones;
  QStringList         textureList;
  unsigned char       floor = 0;
};

#endif // LEVELMAP_H
