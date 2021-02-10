#include "dataengine.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

DataEngine::DataEngine(QObject *parent) : QObject(parent)
{
  //data = QJsonDocument::fromJson("{\"characters\":{},\"levels\":{}}");
  data.insert("characters", characters);
  data.insert("levels", levels);
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
  qDebug() << "dataengine state#1:" << QJsonDocument(data).toJson();
  qDebug() << "dataengine state#2:" << QJsonDocument(levelData).toJson();
  qDebug() << "dataengine state#3:" << QJsonDocument(levels).toJson();
}

StatModel* DataEngine::makeStatModel(const QString& characterId, const QString& source)
{
  QJsonValue characterData = data["characters"].toObject()[characterId];
  StatModel* result = new StatModel(this);

  if (characterData.isUndefined() || characterData.isNull())
  {
    QFile file(":/assets/characterSheets/" + source + ".json");

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
