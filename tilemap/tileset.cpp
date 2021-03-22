#include "tileset.h"
#include <QFile>
#include <QJsonDocument>

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
    source = tilesetsPath + document["image"].toString();
    image = QImage(source);
    spacing = document["spacing"].toInt();
    columns = document["columns"].toInt();
    tileSize.setWidth(document["tilewidth"].toInt());
    tileSize.setHeight(document["tileheight"].toInt());
    sourceSize.setWidth(document["imagewidth"].toInt());
    sourceSize.setHeight(document["imageheight"].toInt());
    tileCount = document["tilecount"].toInt();
    return true;
  }
  else
    qDebug() << "Tileset: failed to open tileset" << filepath;
  return false;
}

QRect Tileset::getClipRectFor(int tileId) const
{
  QRect result;

  tileId -= firstGid;
  if (tileId < tileCount)
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
