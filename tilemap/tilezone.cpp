#include "tilezone.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

TileZone::TileZone(QObject *parent) : QObject(parent)
{
  offset = QPoint(0, 0);
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
    else if (propertyName == "toZone")
      targetZone = value.toString();
  }
  for (QJsonValue value : object["data"].toArray())
  {
    int tid = value.toInt();

    if (tid > 0)
      positions.push_back(currentPosition);
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

bool TileZone::isInside(int x, int y, unsigned char z) const
{
  return positions.indexOf(QPoint(x - offset.x(), y - offset.y())) >= 0 && z == floor;
}

QVector<QPoint> TileZone::getAbsolutePositions() const
{
  QVector<QPoint> points;

  points.reserve(positions.size());
  for (QPoint position : positions)
    points.push_back(position + offset);
  return points;
}

void TileZone::addPosition(QPoint position)
{
  qDebug() << "Adding position" << position << "at offset" << offset << " -> " << (position - offset);
  positions << (position - offset);
  emit tilesChanged();
}

void TileZone::removePosition(QPoint position)
{
  positions.removeAll(position - offset);
  emit tilesChanged();
}

void TileZone::addRelativePosition(QPoint position)
{
  positions << position;
  emit tilesChanged();
}

void TileZone::removeRelativePosition(QPoint position)
{
  positions.removeAll(position);
  emit tilesChanged();
}
