#include "statistics.h"
#include "game.h"

CharacterStatistics::CharacterStatistics(QObject *parent) : ParentType(parent)
{
  connect(this, &CharacterStatistics::characterSheetChanged, this, &CharacterStatistics::onCharacterSheetChanged);
}

void CharacterStatistics::load(const QJsonObject& data)
{
  QString objectName = data["objectName"].toString();

  characterSheet = data["charsheet"].toString(objectName);
  isUnique = data["uniq"].toBool();
  if (isUnique || data["stats"]["name"].toString().length() == 0)
    onCharacterSheetChanged();
  else
  {
    StatModel* charSheet = new StatModel(this);

    charSheet->fromJson(data["stats"].toObject());
    setStatistics(charSheet);
  }
  ParentType::load(data);
}

void CharacterStatistics::save(QJsonObject& data) const
{
  data["charsheet"] = characterSheet;
  data["uniq"] = isUnique;
  if (!(Game::get()->property("isGameEditor").toBool()))
  {
    if (isUnique)
      Game::get()->getDataEngine()->saveStatModel(getObjectName(), statistics);
    else
    {
      QJsonObject statData;

      statistics->toJson(statData);
      data.insert("stats", statData);
    }
  }
}

void CharacterStatistics::setStatistics(StatModel* value)
{
  if (statistics)
  {
    disconnect(statistics, &StatModel::factionChanged, this, &CharacterStatistics::initializeFaction);
    statistics->deleteLater();
  }
  statistics = value;
  connect(statistics, &StatModel::factionChanged, this, &CharacterStatistics::initializeFaction);
  qDebug() << "set stat sheet on" << getObjectName() << ':' << statistics->getName() << " with faction " << statistics->property("faction").toString();
  initializeFaction();
  emit statisticsChanged();
}

void CharacterStatistics::setCharacterSheet(const QString& name)
{
  characterSheet = name;
  emit characterSheetChanged();
}

void CharacterStatistics::onCharacterSheetChanged()
{
  qDebug() << "Changing character sheet";
  StatModel* charSheet;
  const Race* raceController;

  charSheet = Game::get()->getDataEngine()->makeStatModel(getObjectName(), characterSheet);
  charSheet->setParent(this);
  setStatistics(charSheet);
  raceController = charSheet->getRaceController();
  if (raceController)
  {
    setSpriteName(raceController->getSpriteSheet(charSheet));
    moveTo(position.x(), position.y());
    setAnimation("idle");
  }
}
