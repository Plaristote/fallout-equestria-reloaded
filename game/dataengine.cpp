#include "globals.h"
#include "dataengine.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

#ifndef RELEASE_BUILD
static const char* initialGamePath = "./assets/game.json";
#else
static const char* initialGamePath = ":/assets/game.json";
#endif

DataEngine::DataEngine(QObject *parent) : QObject(parent)
{
  data.insert("characters", characters);
  data.insert("levels", levels);
  data.insert("playerParty", QJsonObject());
  data.insert("time", time);
  data.insert("quests", quests);
  data.insert("worldmap", worldmap);
}

void DataEngine::setVariables(const QJsonObject& variablesData)
{
  variables = variablesData;
  data.remove("vars");
  data.insert("vars", variablesData);
}

QJsonObject DataEngine::getTimeData() const
{
  return time;
}

void DataEngine::setTimeData(const QJsonObject& timeData)
{
  time = timeData;
  data.remove("time");
  data.insert("time", time);
}

QJsonObject DataEngine::getWorldDiplomacy() const
{
  return diplomacy;
}

void DataEngine::setWorldDiplomacy(const QJsonObject& diplomacyData)
{
  diplomacy = diplomacyData;
  data.remove("diplomacy");
  data.insert("diplomacy", diplomacy);
}

void DataEngine::registerFaction(const QString& name)
{
  if (!diplomacy.contains(name))
  {
    QJsonObject factionData;

    factionData.insert("enemies", QJsonObject());
    diplomacy.insert(name, factionData);
    data.remove("diplomacy");
    data.insert("diplomacy", diplomacy);
    emit diplomacyUpdated();
  }
}

QStringList DataEngine::getFactionList() const
{
  return diplomacy.keys();
}

QStringList DataEngine::getEnemyListForFaction(const QString &faction) const
{
  QStringList results;
  QJsonObject enemyData = diplomacy[faction]["enemies"].toObject();

  for (const QString& key : enemyData.keys())
  {
    if (enemyData[key].toBool())
      results << key;
  }
  return results;
}

void DataEngine::setFactionAsEnemy(const QString &faction, const QString &enemyFaction, bool set)
{
  QJsonObject factionData = diplomacy[faction].toObject();
  QJsonObject enemyData = factionData["enemies"].toObject();

  enemyData.insert(enemyFaction, set);
  factionData.insert("enemies", enemyData);
  diplomacy.insert(faction, factionData);
  emit diplomacyUpdated();
}

void DataEngine::setFactionReputationEnabled(const QString &faction, bool set)
{
  QJsonObject factionData = diplomacy[faction].toObject();

  if (hasReputation(faction))
    factionData.insert("withReputation", false);
  factionData.insert("reputation", set);
  diplomacy.insert(faction, factionData);
  emit diplomacyUpdated();
}

bool DataEngine::hasFactionReputationEnabled(const QString &faction) const
{
  return diplomacy[faction]["reputation"].toBool(false);
}

void DataEngine::addReputation(const QString &faction, int reputation)
{
  QJsonObject factionData = diplomacy[faction].toObject();

  if (hasFactionReputationEnabled(faction))
    factionData.insert("withReputation", true);
  factionData.insert("reputationValue", factionData["reputationValue"].toInt() + reputation);
  diplomacy.insert(faction, factionData);
  emit diplomacyUpdated();
}

int DataEngine::getReputation(const QString& faction) const
{
  return diplomacy[faction]["reputationValue"].toInt(0);
}

void DataEngine::showReputation(const QString& faction)
{
  if (hasFactionReputationEnabled(faction))
  {
    QJsonObject factionData = diplomacy[faction].toObject();

    factionData.insert("withReputation", true);
    diplomacy.insert(faction, factionData);
    emit diplomacyUpdated();
  }
}

bool DataEngine::hasReputation(const QString& faction) const
{
  return diplomacy[faction]["withReputation"].toBool(false);
}

QJsonObject DataEngine::getQuests() const
{
  return quests;
}

void DataEngine::setQuests(const QJsonObject& questData)
{
  quests = questData;
  data.remove("quests");
  data.insert("quests", quests);
}

QJsonObject DataEngine::getWorldmap() const
{
  return worldmap;
}

void DataEngine::setWorldmap(const QJsonObject& worldmapData)
{
  worldmap = worldmapData;
  data.remove("worldmap");
  data.insert("worldmap", worldmap);
}

void DataEngine::loadFromFile(const QString &path)
{
  QFile in(path == "" ? QString(initialGamePath) : "./saves/" + path);

  if (in.open(QIODevice::ReadOnly))
  {
    auto document = QJsonDocument::fromJson(in.readAll());

    data       = document.object();
    levels     = data["levels"].toObject();
    characters = data["characters"].toObject();
    time       = data["time"].toObject();
    diplomacy  = data["diplomacy"].toObject();
    quests     = data["quests"].toObject();
    worldmap   = data["worldmap"].toObject();
    variables  = data["vars"].toObject();
    emit diplomacyUpdated();
  }
  else
    qDebug() << "/!\\ Could not load save file" << path;
}

void DataEngine::saveToFile(const QString &path)
{
  QFile out(path.startsWith("./assets") ? path : "./saves/" + path);

  qDebug() << "Diplomacy data:" << QJsonDocument(diplomacy).toJson();
  data.insert("diplomacy", diplomacy);
  data.insert("quests", quests);

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

bool DataEngine::hasLevelBeenVisited(const QString& name) const
{
  return isLevelActive(name) && data["levels"][name]["init"] == true;
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
