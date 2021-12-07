#include "savepreview.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QDebug>

SavePreview::SavePreview(QObject *parent) : QObject(parent)
{
  connect(this, &SavePreview::nameChanged, this, &SavePreview::update);
}

void SavePreview::update()
{
  if (name.length() > 0)
  {
    QFile source("./saves/" + name + ".json");

    if (source.open(QIODevice::ReadOnly))
      loadFromJson(source.readAll());
  }
  else
  {
    characterLevel = 0;
    characterName = place = date = time = "";
  }
  emit dataChanged();
}

void SavePreview::loadFromJson(const QByteArray& json)
{
  auto data        = QJsonDocument::fromJson(json).object();
  auto playerParty = data["playerParty"].toObject()["list"].toArray();
  auto player      = playerParty.first()["stats"].toObject();
  auto timeData    = data["time"].toObject();

  characterName  = player["name"].toString();
  characterLevel = player["lvl"].toVariant().toUInt();
  place          = data["level"].toString();
  date           = QString::number(timeData["d"].toInt()) + '/' + QString::number(timeData["M"].toInt()) + '/' + QString::number(timeData["y"].toInt());
  time           = QString::number(timeData["h"].toInt()) + ':' + QString::number(timeData["m"].toInt()) + ':' + QString::number(timeData["s"].toInt());
  qDebug() << "loading avepreview with date" << date << time;
  qDebug() << "time data being" << timeData.toVariantMap();

}

QString SavePreview::screenshotPath() const
{
  return "file:/" + QDir::currentPath() + "/saves/" + name + ".png";
}
