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

    for (QJsonValue value : document["tilesets"].toArray())
    {
      QJsonObject tilesetData = value.toObject();
      auto* tileset  = new Tileset(this);
      auto  firstGid = tilesetData["firstgid"].toInt(1);
      auto  source   = tilemapsPath + tilesetData["source"].toString();

      tilesets.push_back(tileset);
      if (!tileset->load(source, firstGid))
        return false;
    }

    for (QJsonValue value : document["layers"].toArray())
    {
      QJsonObject layerData = value.toObject();
      auto* layer = new TileLayer(this);

      layer->load(layerData, tilesets);
      layers.push_back(layer);
    }

    return true;
  }
  else
    qDebug() << "TileMap: failed to open tilemap" << name;
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
