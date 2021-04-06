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
  {"lights", &TileMap::loadLightFolder}
};

TileMap::TileMap(QObject *parent) : QObject(parent)
{
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
    auto firstGid = tilesetData["firstgid"].toInt(1);
    auto source   = tilemapsPath + sourcePath;
    auto* tileset  = new Tileset(this);

    tilesets.push_back(tileset);
    if (tileset->load(source, firstGid))
      textureList << tileset->getSource();
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
        (this->*loader.value())(layerData);
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

    roofLayer->load(roofLayerData, tilesets);
    roofs.push_back(roofLayer);
  }
}

void TileMap::loadZoneFolder(const QJsonObject& layerData)
{
  const QJsonArray layersData = layerData["layers"].toArray();

  for (QJsonValue value : layersData)
  {
    QJsonObject zoneData = value.toObject();
    auto* zone = new TileZone(this);

    zone->load(zoneData, mapSize);
    zones.push_back(zone);
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