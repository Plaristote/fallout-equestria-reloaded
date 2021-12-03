#include "tilemap.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

static const QString tilemapsPath = "./assets/tilemaps/";

const QMap<QString, TileMap::LayerFolderLoader> TileMap::loaders = {
  {"zones",  &TileMap::loadZoneFolder},
  {"roofs",  &TileMap::loadRoofFolder},
  {"walls",  &TileMap::loadWallFolder},
  {"floors", &TileMap::loadFloorFolder},
  {"lights", &TileMap::loadLightFolder}
};

TileMap::TileMap(QObject *parent) : QObject(parent)
{
}

void TileMap::addTileZone(TileZone* zone)
{
  if (zones.indexOf(zone) < 0)
  {
    zones << zone;
    emit zonesChanged();
  }
}

void TileMap::removeTileZone(TileZone* zone)
{
  if (zones.indexOf(zone) >= 0)
  {
    zones.removeAll(zone);
    emit zonesChanged();
  }
}

void TileMap::addLightLayer(TileLayer* zone)
{
  if (lights.indexOf(zone) < 0)
  {
    lights << zone;
    emit lightsChanged();
  }
}

void TileMap::removeLightLayer(TileLayer* zone)
{
  if (lights.indexOf(zone) >= 0)
  {
    lights.removeAll(zone);
    emit lightsChanged();
  }
}

bool TileMap::load(const QString& name)
{
  QFile sourceFile(tilemapsPath + name + ".json");

  if (sourceFile.open(QIODevice::ReadOnly))
  {
    QJsonDocument document = QJsonDocument::fromJson(sourceFile.readAll());

    tileSize.setWidth(document["tilewidth"].toInt(0));
    tileSize.setHeight(document["tileheight"].toInt(0));
    mapSize.setWidth(document["width"].toInt(0));
    mapSize.setHeight(document["height"].toInt(0));
    loadTilesets(document["tilesets"].toArray());
    loadLayers(document["layers"].toArray());
    return true;
  }
  else
    qDebug() << "TileMap: failed to open tilemap" << (tilemapsPath + name + ".json");
  return false;
}

void TileMap::loadTilesets(const QJsonArray& tilesetsData)
{
  bool hasLightLayer = false;

  for (const QJsonValue& value : tilesetsData)
  {
    QJsonObject tilesetData = value.toObject();
    auto sourcePath = tilesetData["source"].toString();
    auto firstGid   = tilesetData["firstgid"].toInt(1);
    auto source     = tilemapsPath + sourcePath;
    auto* tileset   = new Tileset(this);

    if (tileset->load(source, firstGid))
      textureList << tileset->getSource();
    if (tilesets.count() > 0)
      tilesets.last()->overrideLastGid(firstGid - 1);
    tilesets.push_back(tileset);
    if (tileset->getName() == "lights")
      hasLightLayer = true;
  }
  if (!hasLightLayer)
    loadLightTileset();
}

void TileMap::loadLightTileset()
{
  auto firstGid = tilesets.last()->getLastGid() + 1;
  auto source = tilemapsPath + "../tilesets/lights.json";
  auto* tileset = new Tileset(this);

  tilesets.push_back(tileset);
  if (tileset->load(source, firstGid))
    textureList << tileset->getSource();
}

void TileMap::loadLayers(const QJsonArray& layersData)
{
  for (const QJsonValue& value : layersData)
  {
    QJsonObject layerData = value.toObject();
    auto layerType = layerData["type"].toString();
    auto layerName = layerData["name"].toString();

    if (layerType == "tilelayer")
    {
      auto* layer = new TileLayer(this);

      layer->load(layerData, tilesets);
      layers.push_back(layer);
    }
    else
    {
      auto loader = loaders.find(layerName);

      if (loader != loaders.end())
        (this->**loader)(layerData);
    }
  }
}

void TileMap::loadLightFolder(const QJsonObject& layerData)
{
  const QJsonArray layersData = layerData["layers"].toArray();

  for (const QJsonValue& value : layersData)
  {
    QJsonObject layerData = value.toObject();
    auto* layer = new TileLayer(this);

    layer->load(layerData, tilesets);
    layer->setVisible(layerData["visible"].toBool(false));
    lights.push_back(layer);
  }
}

void TileMap::loadRoofFolder(const QJsonObject& layerData)
{
  const QJsonArray layersData = layerData["layers"].toArray();

  for (const QJsonValue& value : layersData)
  {
    QJsonObject roofLayerData = value.toObject();
    auto* roofLayer = new TileLayer(this);

    roofLayerData["offsetx"] = layerData["offsetx"].toInt() + roofLayerData["offsetx"].toInt();
    roofLayerData["offsety"] = layerData["offsety"].toInt() + roofLayerData["offsety"].toInt();
    roofLayer->load(roofLayerData, tilesets);
    roofs.push_back(roofLayer);
  }
}

void TileMap::loadZoneFolder(const QJsonObject& layerData)
{
  const QJsonArray layersData = layerData["layers"].toArray();

  for (const QJsonValue& value : layersData)
  {
    QJsonObject zoneData = value.toObject();
    auto* zone = new TileZone(this);

    zone->setFloor(floor);
    zone->load(zoneData, mapSize);
    zones.push_back(zone);
  }
}

QJsonArray mergeLayerData(const QJsonArray& a, const QJsonArray& b)
{
  QJsonArray result;

  if (a.size() > 0 && b.size() > 0)
  {
    for (int i = 0 ; i < a.size() ; ++i)
    {
      int valueA = a[i].toInt();
      int valueB = b[i].toInt();

      result.push_back(valueA > 0 ? valueA : valueB);
    }
    return result;
  }
  return a.size() == 0 ? b : a;
}

void TileMap::loadWallFolder(const QJsonObject& layerData)
{
  const QJsonArray layersData = layerData["layers"].toArray();
  auto* wallsV = new TileLayer(this);
  auto* wallsH = new TileLayer(this);
  QStringList vDirections{"north", "south"};
  QStringList hDirections{"east", "west"};
  QJsonObject vLayer{{"name","walls-v"}};
  QJsonObject hLayer{{"name","walls-h"}};

  for (const QJsonValue& value : layersData)
  {
    QString name = value["name"].toString();
    QJsonObject* wallLayer;

    if (vDirections.contains(name, Qt::CaseInsensitive))
      wallLayer = &vLayer;
    else if (hDirections.contains(name, Qt::CaseInsensitive))
      wallLayer = &hLayer;
    else
    {
      qDebug() << "/!\\ Invalid wall layer" << name;
      continue ;
    }
    wallLayer->insert("width",  value["width"].toInt());
    wallLayer->insert("height", value["height"].toInt());
    wallLayer->insert("data",   mergeLayerData(value["data"].toArray(), wallLayer->value("data").toArray()));
  }
  wallsV->load(vLayer, tilesets);
  wallsH->load(hLayer, tilesets);
  layers.push_back(wallsV);
  layers.push_back(wallsH);
}

void TileMap::loadFloorFolder(const QJsonObject& layerData)
{
  const QJsonArray layersData = layerData["layers"].toArray();

  for (const QJsonValue& value : layersData)
  {
    FloorLayer* floor = new FloorLayer(this);
    TileMap*    previousFloor = this;

    floor->load(value.toObject(), this);
    floor->getTileMap()->floor = static_cast<unsigned char>(floors.size() + 1);
    if (floors.size() > 0)
      previousFloor = floors.last()->getTileMap();
    previousFloor->roofs.push_back(floor);
    floors.push_back(floor);
  }
}

Tileset* TileMap::getTileset(const QString &name) const
{
  for (Tileset* tileset : tilesets)
  {
    if (tileset->getName() == name)
      return tileset;
  }
  return nullptr;
}


TileLayer* TileMap::getLayer(const QString &name)
{
  for (TileLayer* layer : qAsConst(layers))
  {
    if (layer->getName() == name)
      return layer;
  }
  return nullptr;
}

TileLayer* TileMap::getRoofLayer(const QString &name)
{
  for (TileLayer* layer : qAsConst(roofs))
  {
    if (layer->getName() == name)
      return layer;
  }
  return nullptr;
}

TileLayer* TileMap::getLightLayer(const QString &name)
{
  for (TileLayer* layer : qAsConst(lights))
  {
    if (layer->getName() == name)
      return layer;
  }
  return nullptr;
}

TileZone* TileMap::getZone(const QString& name)
{
  for (TileZone* zone : qAsConst(zones))
  {
    if (zone->getName() == name)
      return zone;
  }
  return nullptr;
}

TileMap* TileMap::getFloor(const QString& name)
{
  for (FloorLayer* floor : qAsConst(floors))
  {
    if (floor->getName() == name)
      return floor->getTileMap();
  }
  return nullptr;
}

QPoint TileMap::getPointFor(int x, int y) const
{
  return QPoint(
    x * tileSize.width()  / 2 - y * tileSize.width()  / 2,
    y * tileSize.height() / 2 + x * tileSize.height() / 2
  );
}

void TileMap::renderToFile(const QString &filename)
{
  QRect    renderedRect(0, 0, mapSize.width() * tileSize.width(), mapSize.height() * tileSize.height());
  QSize    imageSize = renderedRect.size();
  QImage   image(imageSize, QImage::Format_ARGB32);

  image.fill(Qt::transparent);
  renderToImage(image);
  image.save(filename, "png", 9);
}
