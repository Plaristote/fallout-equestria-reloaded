#include "diplomacy.h"
#include "game.h"

CharacterDiplomacy::CharacterDiplomacy(QObject *parent) : ParentType(parent)
{
  connect(this, &CharacterStatistics::statisticsChanged, this, &CharacterDiplomacy::initializeFaction);
}

void CharacterDiplomacy::load(const QJsonObject& data)
{
  enemyFlag = static_cast<unsigned int>(data["enemyFlag"].toInt(0));
  ParentType::load(data);
}

void CharacterDiplomacy::save(QJsonObject& data) const
{
  data["enemyFlag"] = static_cast<int>(enemyFlag);
  ParentType::save(data);
}

void CharacterDiplomacy::initializeFaction()
{
  auto* characterSheet = getStatistics();
  auto* diplomacy = Game::get()->getDiplomacy();

  disconnect(factionListener);
  if (characterSheet && diplomacy)
  {
    factionListener = connect(characterSheet, &StatModel::factionChanged, this, &CharacterDiplomacy::initializeFaction);
    faction = diplomacy->getFaction(characterSheet->getFaction());
  }
  else
    faction = nullptr;
}

bool CharacterDiplomacy::isAlly(const CharacterDiplomacy* other) const
{
  return other && faction && faction->flag == other->getFactionFlag();
}

bool CharacterDiplomacy::isEnemy(const CharacterDiplomacy* other) const
{
  if (other)
  {
    if (faction)
    {
      auto otherFlag = other->getFactionFlag();

      return otherFlag == 0 ? other->isEnemy(this) : (faction->enemyMask & otherFlag) != 0;
    }
    return (enemyFlag & other->getFactionFlag()) != 0;
  }
  return false;
}

void CharacterDiplomacy::setAsEnemy(CharacterDiplomacy* other)
{
  if (other && !isEnemy(other))
  {
    auto* diplomacy = Game::get()->getDiplomacy();
    auto* faction   = diplomacy->getFaction(other->getFactionFlag());

    if (other->getFactionName() != "") {
      if (getFactionFlag() > 0)
        diplomacy->setAsEnemy(true, getFactionFlag(), other->getFactionFlag());
      else
        enemyFlag += other->getFactionFlag();
    }
    else if (faction)
      other->setAsEnemy(this);
    emit diplomacyUpdated();
  }
}

void CharacterDiplomacy::setAsFriendly(CharacterDiplomacy* other)
{
  if (other && isEnemy(other))
  {
    auto* diplomacy = Game::get()->getDiplomacy();
    auto* faction   = diplomacy->getFaction(other->getFactionFlag());

    if (other->getFactionName() != "") {
      if (getFactionFlag() > 0)
        diplomacy->setAsEnemy(false, getFactionFlag(), other->getFactionFlag());
      else
        enemyFlag -= other->getFactionFlag();
    }
    else if (faction)
      other->setAsFriendly(this);
    emit diplomacyUpdated();
  }
}

int CharacterDiplomacy::getReputation() const
{
  return Game::get()->getDataEngine()->getReputation(getFactionName());
}

void CharacterDiplomacy::addReputation(int value) const
{
  Game::get()->getDataEngine()->addReputation(getFactionName(), value);
}
