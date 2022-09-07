#include "worldmapcity.h"
#include <QJsonObject>
#include <cmath>

WorldMapCity::WorldMapCity(QObject* parent) : QObject(parent)
{
  splashscreen = new CitySplashscreen(this);
}

void WorldMapCity::load(const QJsonObject& data)
{
  setName(data["name"].toString());
  setLevel(data["level"].toString());
  setPosition(QPoint(data["x"].toInt(), data["y"].toInt()));
  setSize(data["size"].toInt());
  if (data["splashscreen"].isObject())
    splashscreen->load(data["splashscreen"].toObject());
}

void WorldMapCity::save(QJsonObject& data)
{
  QJsonObject splashscreenData;

  splashscreen->save(splashscreenData);
  data.insert("name",  getName());
  data.insert("level", getLevel());
  data.insert("x",     position.x());
  data.insert("y",     position.y());
  data.insert("size",  getSize());
  data.insert("splashscreen", splashscreenData);
}

bool WorldMapCity::isInside(QPoint point)
{
  auto a = position.x() - point.x();
  auto b = position.y() - point.y();

  auto patchedSize = size - 10; // for some reasons, drawn circle are smaller than they should be ?

  return std::sqrt(a * a + b * b) < patchedSize;
}

void CitySplashscreen::load(const QJsonObject& data)
{
  auto map = data["entryPoints"].toVariant().toMap();

  for (auto it = map.begin() ; it != map.end() ; ++it)
    entryPoints.insert(it.key(), QPoint(it->toJsonObject()["x"].toInt(), it->toJsonObject()["y"].toInt()));
  imagePath = data["imagePath"].toString("");
}

void CitySplashscreen::save(QJsonObject& data)
{
  QJsonObject entryPointsData;

  for (auto it = entryPoints.begin() ; it != entryPoints.end() ; ++it)
  {
    if (it.key().length() > 0)
      entryPointsData.insert(it.key(), QJsonObject{{"x", it->x()}, {"y", it->y()}});
  }
  data["imagePath"] = imagePath;
  data["entryPoints"] = entryPointsData;
}

void CitySplashscreen::setPositionFor(const QString& key, QPoint position)
{
  entryPoints.insert(key, position); emit entryPointsChanged();
}
void CitySplashscreen::setEntryPointAt(const QString& key, const QString& level)
{
  entryPoints.insert(level, entryPoints[key]);
  //entryPoints.remove(key);
  emit entryPointsChanged();
}
