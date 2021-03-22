#include "tilemap.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

static const QString tilemapsPath = "./assets/tilemaps/";

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

    qDebug() << "Loading tilemap" << name << '(' << mapSize.width() << ',' << mapSize.height() << ')';

    // BEGIN TILEsETs LOADING
    for (QJsonValue value : document["tilesets"].toArray())
    {
      QJsonObject tilesetData = value.toObject();
      auto sourcePath = tilesetData["source"].toString();
      auto firstGid = tilesetData["firstgid"].toInt(1);
      auto source   = tilemapsPath + sourcePath;

      // OBJECT TILEsET
      if (sourcePath.indexOf("/objects-") >= 0)
      {
        auto* tileset = new ObjectTileset(this);

        qDebug() << "Le object tileset ma gueule" << sourcePath;
        objectTilesets.push_back(tileset);
        if (tileset->load(source, firstGid))
          textureList.append(tileset->getAllSources());
        else
          return false;
      }
      // REGULAR TILEsET
      else
      {
        auto* tileset  = new Tileset(this);

        qDebug() << "Registering new tileset" << sourcePath;
        tilesets.push_back(tileset);
        if (tileset->load(source, firstGid))
          textureList << tileset->getSource();
        else
          return false;
      }
    }

    for (QJsonValue value : document["layers"].toArray())
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
        if (layerName == "zones")
        {
          for (QJsonValue value : layerData["layers"].toArray())
          {
            QJsonObject zoneData = value.toObject();
            auto* zone = new TileZone(this);

            zone->load(zoneData, mapSize);
            zones.push_back(zone);
          }
        }
        else if (layerName == "roofs")
        {
          for (QJsonValue value : layerData["layers"].toArray())
          {
            QJsonObject roofLayerData = value.toObject();
            auto* roofLayer = new TileLayer(this);

            roofLayer->load(roofLayerData, tilesets);
            roofs.push_back(roofLayer);
          }
        }
      }
    }
    return true;
  }
  else
    qDebug() << "TileMap: failed to open tilemap" << (tilemapsPath + name + ".json");
  return false;
}

TileLayer* TileMap::getLayer(const QString &name)
{
  for (TileLayer* layer : layers)
  {
    if (layer->getName() == name)
      return layer;
  }
  return nullptr;
}

TileLayer* TileMap::getRoofLayer(const QString &name)
{
  for (TileLayer* layer : roofs)
  {
    if (layer->getName() == name)
      return layer;
  }
  return nullptr;
}

TileZone* TileMap::getZone(const QString& name)
{
  for (TileZone* zone : zones)
  {
    if (zone->getName() == name)
      return zone;
  }
  return nullptr;
}

QString TileMap::getObjectSource(int gid) const
{
  for (auto* tileset : objectTilesets)
  {
    if (tileset->getFirstGid() <= gid && tileset->getLastGid() > gid)
      return tileset->getSource(gid);
  }
  return QString();
}

QSize TileMap::getObjectSize(int gid) const
{
  for (auto* tileset : objectTilesets)
  {
      if (tileset->getFirstGid() <= gid && tileset->getLastGid() > gid)
        return tileset->getTileSize(gid);
  }
  return QSize();
}
