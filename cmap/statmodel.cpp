#include "statmodel.h"

StatModel::StatModel(QObject *parent) : QObject(parent)
{
  strength = perception = endurance = charisma = intelligence = agility = luck = 5;
  hitPoints = 1;
  skillPoints = 0;
  experience = 0;
  connect(this, &StatModel::specialChanged, this, &StatModel::updateBaseValues);
}

int StatModel::getXpNextLevel() const
{
  int level = this->level + 1;

  return ((level * (level - 1) / 2) * 1000);
}

void StatModel::addExperience(int xp)
{
  experience += xp;
  if (experience >= getXpNextLevel())
    levelUp();
  emit experienceChanged();
}

void StatModel::levelUp()
{
  level       += 1;
  skillPoints += get_skillRate();
  lastPerk    += 1;
  hitPoints   += endurance / 3 > 0 ? 1 : endurance / 3;
  if (lastPerk >= get_perkRate())
  {
    availablePerks++;
    lastPerk = 0;
    emit availablePerksChanged();
  }
  emit levelChanged();
  emit skillPointsChanged();
}

void StatModel::updateBaseValues()
{
  data.actionPoints        = agility < 5 ? 5 : agility;
  data.armorClass          = agility;
  data.carryWeight         = 25 + strength * 25;
  data.criticalChance      = luck;
  data.damageResistance    = 0;
  data.healingRate         = endurance / 3 > 1 ? endurance / 3 : 1;
  data.maxHitPoints        = 15 + strength + 2 * endurance + level * 3;
  data.meleeDamage         = strength - 5 > 1 ? strength - 5 : 1;
  data.perkRate            = 3;
  data.poisonResistance    = endurance * 5;
  data.radiationResistance = (endurance - 1) * 2;
  data.skillRate           = 5 + intelligence * 2;

  data.smallGuns    = 5 + 4 * agility;
  data.bigGuns      = strength + 2 * agility;
  data.energyGuns   = intelligence + 2 * agility;
  data.explosives   = 10 + perception + agility;
  data.unarmed      = 30 + (2 * (agility + strength));
  data.meleeWeapons = 20 + (2 * (agility + strength));
  data.lockpick     = 20 + (5 * perception) + (5 * agility);
  data.medicine     = 15 + perception + intelligence;
  data.repair       = perception + (3 * intelligence);
  data.science      = 4 * intelligence;
  data.sneak        = 5 + 3 * agility;
  data.spellcasting = 25 + (2 * intelligence) + luck;
  data.steal        = 3 * agility;
  data.barter       = 4 * charisma;
  data.outdoorsman  = 5 + (2 * endurance) + (2 * intelligence);
  data.speech       = 5 * charisma + intelligence;
  data.gambling     = charisma + 4 * luck;

  emit statisticsChanged();
}
