#include "statistics.h"
#include "game.h"
#include <QDebug>

CharacterStatistics::CharacterStatistics(QObject *parent) : ParentType(parent)
{
  connect(this, &CharacterStatistics::characterSheetChanged, this, &CharacterStatistics::onCharacterSheetChanged);
  connect(this, &CharacterStatistics::characterSheetChanged, this, &CharacterStatistics::updateSpriteSheet);
  connect(inventory, &Inventory::equippedItemsChanged,       this, &CharacterStatistics::updateSpriteSheet);
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
  ParentType::save(data);
}

void CharacterStatistics::setStatistics(StatModel* value)
{
  if (statistics)
  {
    disconnect(statistics, &StatModel::factionChanged, this, &CharacterStatistics::initializeFaction);
    statistics->deleteLater();
  }
  statistics = value;
  if (value)
  {
    connect(statistics, &StatModel::factionChanged, this, &CharacterStatistics::initializeFaction);
    initializeFaction();
    emit statisticsChanged();
  }
}

void CharacterStatistics::setCharacterSheet(const QString& name)
{
  characterSheet = name;
  emit characterSheetChanged();
}

void CharacterStatistics::onCharacterSheetChanged()
{
  StatModel* charSheet;

  charSheet = Game::get()->getDataEngine()->makeStatModel(getObjectName(), characterSheet);
  charSheet->setParent(this);
  setStatistics(charSheet);
}

void CharacterStatistics::updateSpriteSheet()
{
  if (statistics)
  {
    const Race* raceController = statistics->getRaceController();
    auto* animationLibrary = AnimationLibrary::get();

    if (raceController)
    {
      CharacterSpriteDescriptor descriptor = raceController->getSpriteSheet(statistics);
      InventoryItem* armor = inventory->getEquippedItem("armor");

      if (armor)
        descriptor.armor = armor->getAnimation();
      if (descriptor.layered)
      {
        animationLibrary->registerCharacterSpriteSheet(descriptor);
        setSpriteName(animationLibrary->getCharacterSpriteName(descriptor));
      }
      else
        setSpriteName(descriptor.base);
      moveTo(getPoint());
      setAnimation("idle");
    }
  }
}
