#include "globals.h"
#include "dataengine.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

DataEngine::DataEngine(QObject *parent) : QObject(parent)
{
  data.insert("characters", characters);
  data.insert("levels", levels);
  data.insert("playerParty", QJsonObject());
  data.insert("time", time);
}

QJsonObject DataEngine::getTimeData() const
{
  return time;
}

void DataEngine::setTimeData(const QJsonObject& timeData)
{
  time = timeData;
  data.remove("time");
  data.insert("time", timeData);
}

void DataEngine::loadFromFile(const QString &path)
{
  QFile in(path.startsWith("./assets") ? path : "./saves/" + path);

  if (in.open(QIODevice::ReadOnly))
  {
    auto document = QJsonDocument::fromJson(in.readAll());

    data       = document.object();
    levels     = data["levels"].toObject();
    characters = data["characters"].toObject();
    time       = data["time"].toObject();
  }
  else
    qDebug() << "Could not load save file" << path;
}

void DataEngine::saveToFile(const QString &path)
{
  QFile out(path.startsWith("./assets") ? path : "./saves/" + path);

  if (out.open(QIODevice::WriteOnly))
    out.write(QJsonDocument(data).toJson());
  else
    qDebug() << "Could not open save file" << path;
}

QJsonObject DataEngine::getPlayerParty() const
{
  return data["playerParty"].toObject();
}

void DataEngine::setPlayerParty(const QJsonObject& partyData)
{
  data.remove("playerParty");
  data.insert("playerParty", partyData);
}

void DataEngine::setCurrentLevel(const QString& name)
{
  data["level"] = name;
}

QString DataEngine::getCurrentLevel() const
{
  return data["level"].toString("");
}

void DataEngine::exitLevel()
{
  data.remove("level");
}

bool DataEngine::isLevelActive(const QString& name) const
{
  return data["levels"][name].isObject();
}

QJsonObject DataEngine::getLevelData(const QString& name)
{
  return data["levels"].toObject()[name].toObject();
}

void DataEngine::setLevelData(const QString& name, const QJsonObject& levelData)
{
  levels.remove(name);
  levels.insert(name, levelData);
  data.insert("levels", levels);
}

StatModel* DataEngine::makeStatModel(const QString& characterId, const QString& source)
{
  QJsonValue characterData = data["characters"].toObject()[characterId];
  StatModel* result = new StatModel(this);

  if (characterData.isUndefined() || characterData.isNull())
  {
    QFile file(ASSETS_PATH + "characterSheets/" + source + ".json");

    if (file.open(QIODevice::ReadOnly))
    {
      QJsonDocument document = QJsonDocument::fromJson(file.readAll());

      result->fromJson(document.object());
    }
    else
      qDebug() << "DataEngine: failed to find character sheet" << source;
  }
  else
    result->fromJson(characterData.toObject());
  return result;
}

void DataEngine::saveStatModel(const QString &characterId, StatModel* model)
{
  QJsonObject characterData;

  model->toJson(characterData);
  characters.remove(characterId);
  characters.insert(characterId, characterData);
  data.insert("charaters", characters);
}
