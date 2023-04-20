#include "tileset.h"
#include "properties.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

static const QString tilesetsPath = "./assets/tilesets/";

Tileset::Tileset(QObject *parent) : QObject(parent)
{

}

bool Tileset::load(const QString& filepath, int firstGid)
{
  QFile sourceFile(filepath);

  this->firstGid = firstGid;
  if (sourceFile.open(QIODevice::ReadOnly))
  {
    QJsonDocument document = QJsonDocument::fromJson(sourceFile.readAll());

    this->name = document["name"].toString();
    tiledVersion = document["tiledversion"].toString();
    source = tilesetsPath + document["image"].toString();
    image = QImage(source);
    spacing = document["spacing"].toInt();
    columns = document["columns"].toInt();
    tileSize.setWidth(document["tilewidth"].toInt());
    tileSize.setHeight(document["tileheight"].toInt());
    sourceSize.setWidth(document["imagewidth"].toInt());
    sourceSize.setHeight(document["imageheight"].toInt());
    tileCount = document["tilecount"].toInt();
    loadProperties(document);
    return true;
  }
  else
    qDebug() << "Tileset: failed to open tileset" << filepath;
  return false;
}

void Tileset::loadProperties(const QJsonDocument& document)
{
  const QJsonArray tiles = document["tiles"].toArray();

  for (const QJsonValue& descriptors : tiles)
  {
    int tileId = descriptors["id"].toInt();

    tileProperties.insert(tileId, loadTiledProperties(descriptors.toObject(), tiledVersion));
  }
}

QRect Tileset::getClipRectFor(int tileId) const
{
  QRect result;

  tileId -= firstGid;
  if (tileId < tileCount || tileId <= 0)
  {
    int row = tileId / columns;
    int col = tileId % columns;

    result.setX(col * (tileSize.width() + spacing));
    result.setY(row * (tileSize.height() + spacing));
    result.setSize(tileSize);
  }
  else
    qDebug() << "Tileset:" << name << ": out of range tile" << tileId;
  return result;
}

QVariant Tileset::getProperty(int tileId, const QByteArray &name) const
{
  tileId -= firstGid;
  if (tileProperties.contains(tileId))
  {
    const QVariantMap& properties = tileProperties[tileId];

    if (properties.contains(name))
      return properties[name];
  }
  return QVariant();
}
