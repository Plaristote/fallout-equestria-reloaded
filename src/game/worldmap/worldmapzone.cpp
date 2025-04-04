#include "worldmapzone.h"
#include <QJsonArray>

WorldMapZone::WorldMapZone(QObject *parent) : QObject(parent)
{
  movementSpeed = 5;
}

void WorldMapZone::addCase(QPoint position)
{
  if (cases.indexOf(position) < 0)
  {
    cases << position;
    emit casesChanged();
  }
}

void WorldMapZone::removeCase(QPoint position)
{
  auto index = cases.indexOf(position);

  if (index >= 0)
  {
    cases.removeAt(index);
    emit casesChanged();
  }
}

void WorldMapZone::load(const QJsonObject& data)
{
  const QJsonArray casesJson = data["cases"].toArray();

  name = data["name"].toString();
  movementSpeed = data["movementSpeed"].toInt(0);
  for (const QJsonValue& caseJson : casesJson)
    cases << QPoint(caseJson["x"].toInt(), caseJson["y"].toInt());
}

QJsonObject WorldMapZone::save() const
{
  QJsonObject data;
  QJsonArray casesJson;

  data["name"] = name;
  data["movementSpeed"] = movementSpeed;
  for (const QPoint& item : cases)
  {
    QJsonObject caseJson;

    caseJson["x"] = item.x();
    caseJson["y"] = item.y();
    casesJson << caseJson;
  }
  data.insert("cases", casesJson);
  return data;
}
