#include "objecttileset.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

ObjectTileset::ObjectTileset(QObject *parent) : QObject(parent)
{

}

bool ObjectTileset::load(const QString& filepath, int firstGid)
{
  QFile file(filepath);

  this->firstGid = firstGid;
  if (file.open(QIODevice::ReadOnly))
  {
    auto document = QJsonDocument::fromJson(file.readAll());

    this->name = document["name"].toString();
    qDebug() << "ObjectTileset new" << this->name;
    for (QJsonValue value : document["tiles"].toArray())
    {
      QJsonObject objectTileData = value.toObject();
      Tile objectTile;

      objectTile.source = "./assets" + objectTileData["image"].toString().replace("../", "/");
      objectTile.size.setWidth(objectTileData["imagewidth"].toInt());
      objectTile.size.setHeight(objectTileData["imageheight"].toInt());
      tiles.insert(objectTileData["id"].toInt(), objectTile);
      qDebug() << "-> new tile inserted" << objectTile.source;
    }
    return true;
  }
  return false;
}

QString ObjectTileset::getSource(int gid) const
{
  int  id = gid - firstGid;
  auto it = tiles.find(id);

  if (it != tiles.end())
    return it->source;
  return "";
}

QSize ObjectTileset::getTileSize(int gid) const
{
  int  id = gid - firstGid;
  auto it = tiles.find(id);
  QSize result;

  if (it != tiles.end())
    result = it->size;
  return result;
}

QStringList ObjectTileset::getAllSources() const
{
  QStringList result;

  for (const auto& tile : tiles)
    result << tile.source;
  return result;
}
