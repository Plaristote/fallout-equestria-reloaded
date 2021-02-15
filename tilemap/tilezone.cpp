#include "tilezone.h"
#include <QJsonObject>
#include <QJsonArray>

TileZone::TileZone(QObject *parent) : QObject(parent)
{
  clippedRect.setX(72 * 2); clippedRect.setY(0);
  clippedRect.setWidth(72);
  clippedRect.setHeight(36);
}

void TileZone::load(const QJsonObject& object, const QSize mapSize)
{
  QPoint currentPosition(0, 0);

  name = object["name"].toString();
  for (QJsonValue property : object["properties"].toArray())
  {
    QString propertyName = property["name"].toString();
    QJsonValue value = property["value"];

    if (propertyName == "type")
      type = value.toString();
    else if (propertyName == "default")
      isDefault = value.toBool();
    else if (propertyName == "target")
      target = value.toString();
  }
  for (QJsonValue value : object["data"].toArray())
  {
    int tid = value.toInt();

    if (tid > 0)
      tiles.push_back(currentPosition);
    if (currentPosition.x() >= mapSize.width() - 1)
    {
      currentPosition.setX(0);
      currentPosition.ry()++;
    }
    else
      currentPosition.rx()++;
  }
  if (type == "exit")
  {
    clippedRect.setX(0);
    clippedRect.setY(0);
    if (target == "")
      clippedRect.setX(72);
    clippedRect.setWidth(72);
    clippedRect.setHeight(36);
  }
  emit tilesChanged();
}

bool TileZone::isInside(int x, int y) const
{
  return tiles.indexOf(QPoint(x, y)) >= 0;
}

void TileZone::addPosition(QPoint position)
{
  tiles << position;
  emit tilesChanged();
}

void TileZone::removePosition(QPoint position)
{
  tiles.removeAll(position);
  emit tilesChanged();
}

