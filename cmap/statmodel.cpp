#include "statmodel.h"
#include <QJsonArray>

#include "game.h"

StatModel::StatModel(QObject *parent) : QObject(parent)
{
  strength = perception = endurance = charisma = intelligence = agility = luck = 5;
  hitPoints = 1;
  skillPoints = 0;
  experience = 0;
  connect(this, &StatModel::specialChanged, this, &StatModel::updateBaseValues);
  connect(this, &StatModel::traitsChanged,  this, &StatModel::updateBaseValues);
  connect(this, &StatModel::specialChanged, this, &StatModel::acceptableChanged);
  connect(this, &StatModel::traitsChanged,  this, &StatModel::acceptableChanged);
  connect(this, &StatModel::nameChanged,    this, &StatModel::acceptableChanged);
}

QStringList StatModel::getAvailableRaces() const
{
  return QStringList() << "Earth pony" << "Unicorn" << "Pegasus";
}

QStringList StatModel::getGenders() const
{
  return QStringList() << "Stallion" << "Mare";
}

int StatModel::getXpNextLevel() const
{
  int level = this->level + 1;

  return ((level * (level - 1) / 2) * 1000);
}

void StatModel::addKill(const QString &race, unsigned int amount)
{
  if (kills.contains(race))
    kills[race] += amount;
  else
    kills.insert(race, amount);
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
  hitPoints   += 3;
  if (lastPerk >= get_perkRate())
  {
    availablePerks++;
    lastPerk = 0;
    emit availablePerksChanged();
  }
  updateBaseValues();
  emit levelChanged();
  emit skillPointsChanged();
  emit hitPointsChanged();
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

  auto allTraits = Game::get()->getCmapTraits();

  for (auto trait : allTraits)
  {
    if  (traits.contains(trait.name))
    {
      data.actionPoints        = trait.modifyBaseStatistic(this, "actionPoints",        data.actionPoints);
      data.armorClass          = trait.modifyBaseStatistic(this, "armorClass",          data.armorClass);
      data.carryWeight         = trait.modifyBaseStatistic(this, "carryWeight",         data.carryWeight);
      data.criticalChance      = trait.modifyBaseStatistic(this, "criticalChance",      data.criticalChance);
      data.damageResistance    = trait.modifyBaseStatistic(this, "damageResistance",    data.damageResistance);
      data.healingRate         = trait.modifyBaseStatistic(this, "healingRate",         data.healingRate);
      data.maxHitPoints        = trait.modifyBaseStatistic(this, "maxHitPoints",        data.maxHitPoints);
      data.meleeDamage         = trait.modifyBaseStatistic(this, "meleeDamage",         data.meleeDamage);
      data.perkRate            = trait.modifyBaseStatistic(this, "perkRate",            data.perkRate);
      data.poisonResistance    = trait.modifyBaseStatistic(this, "poisonResistance",    data.poisonResistance);
      data.radiationResistance = trait.modifyBaseStatistic(this, "radiationResistance", data.radiationResistance);
      data.skillRate           = trait.modifyBaseStatistic(this, "skillRate",           data.skillRate);

      data.smallGuns    = trait.modifyBaseSkill(this, "smallGuns",    data.smallGuns);
      data.bigGuns      = trait.modifyBaseSkill(this, "bigGuns",      data.bigGuns);
      data.energyGuns   = trait.modifyBaseSkill(this, "energyGuns",   data.energyGuns);
      data.explosives   = trait.modifyBaseSkill(this, "explosives",   data.explosives);
      data.unarmed      = trait.modifyBaseSkill(this, "unarmed",      data.unarmed);
      data.meleeWeapons = trait.modifyBaseSkill(this, "meleeWeapons", data.meleeWeapons);
      data.lockpick     = trait.modifyBaseSkill(this, "lockpick",     data.lockpick);
      data.medicine     = trait.modifyBaseSkill(this, "medicine",     data.medicine);
      data.repair       = trait.modifyBaseSkill(this, "repair",       data.repair);
      data.science      = trait.modifyBaseSkill(this, "science",      data.science);
      data.sneak        = trait.modifyBaseSkill(this, "sneak",        data.sneak);
      data.spellcasting = trait.modifyBaseSkill(this, "spellcasting", data.spellcasting);
      data.steal        = trait.modifyBaseSkill(this, "steal",        data.steal);
      data.barter       = trait.modifyBaseSkill(this, "barter",       data.barter);
      data.outdoorsman  = trait.modifyBaseSkill(this, "outdoorsman",  data.outdoorsman);
      data.speech       = trait.modifyBaseSkill(this, "speech",       data.speech);
      data.gambling     = trait.modifyBaseSkill(this, "gambling",     data.gambling);
    }
  }

  emit statisticsChanged();
}

bool StatModel::isAcceptable() const
{
  return traits.length() == getMaxTraits() && specialPoints == 0 && name.length() > 0;
}

QStringList StatModel::getAvailableTraits()
{
  QStringList results;
  auto traits = Game::get()->getCmapTraits();

  for (auto trait : traits)
    results << trait.name;
  return results;
}

void StatModel::toggleTrait(const QString& name, bool value)
{
  auto traits = Game::get()->getCmapTraits();

  for (auto trait : traits)
  {
    if  (trait.name == name)
    {
      trait.toogle(this, value);
      break ;
    }
  }
}

void StatModel::toggleFaceAccessory(const QString& name)
{
  if (faceAccessories.indexOf(name) >= 0)
    faceAccessories.removeAll(name);
  else
    faceAccessories << name;
  emit faceAccessoriesChanged();
}

void StatModel::confirmChanges()
{
  spentPoints = SkillData();
}

void StatModel::cancelChanges()
{
  skillPoints += (
    spentPoints.smallGuns +
    spentPoints.bigGuns +
    spentPoints.energyGuns +
    spentPoints.explosives +
    spentPoints.unarmed +
    spentPoints.lockpick +
    spentPoints.meleeWeapons +
    spentPoints.medicine +
    spentPoints.repair +
    spentPoints.science +
    spentPoints.sneak +
    spentPoints.spellcasting +
    spentPoints.steal +
    spentPoints.barter +
    spentPoints.outdoorsman +
    spentPoints.speech +
    spentPoints.gambling
  );
  modifiers.smallGuns    -= spentPoints.smallGuns;
  modifiers.bigGuns      -= spentPoints.bigGuns;
  modifiers.energyGuns   -= spentPoints.energyGuns;
  modifiers.explosives   -= spentPoints.explosives;
  modifiers.unarmed      -= spentPoints.unarmed;
  modifiers.lockpick     -= spentPoints.lockpick;
  modifiers.meleeWeapons -= spentPoints.meleeWeapons;
  modifiers.medicine     -= spentPoints.medicine;
  modifiers.repair       -= spentPoints.repair;
  modifiers.science      -= spentPoints.science;
  modifiers.sneak        -= spentPoints.sneak;
  modifiers.spellcasting -= spentPoints.spellcasting;
  modifiers.steal        -= spentPoints.steal;
  modifiers.barter       -= spentPoints.barter;
  modifiers.outdoorsman  -= spentPoints.outdoorsman;
  modifiers.speech       -= spentPoints.speech;
  modifiers.gambling     -= spentPoints.gambling;
  spentPoints = SkillData();
  emit statisticsChanged();
  emit skillPointsChanged();
}

void StatModel::fromJson(const QJsonObject& json)
{
  const QJsonArray jsonTraits = json["traits"].toArray();
  const QJsonArray jsonPerks  = json["perks"].toArray();

  name = json["name"].toString();
  age  = json["age"].toInt(21);
  race           = json["race"].toString();
  gender         = json["gender"].toString();
  hitPoints      = json["hp"].toInt(1);
  level          = json["lvl"].toInt();
  experience     = json["xp"].toInt();
  skillPoints    = json["skill-points"].toInt(0);
  availablePerks = json["available-perks"].toInt(0);
  lastPerk = json["last-perk"].toInt(0);
  strength     = json["str"].toInt(5);
  endurance    = json["end"].toInt(5);
  perception   = json["per"].toInt(5);
  charisma     = json["cha"].toInt(5);
  intelligence = json["int"].toInt(5);
  agility      = json["agi"].toInt(5);
  luck         = json["luc"].toInt(5);
  faction      = json["faction"].toString();
  traits.clear();
  perks.clear();
  for (const QJsonValue& value : jsonTraits)
    traits.push_back(value.toString());
  for (const QJsonValue& value : jsonPerks)
    perks.push_back(value.toString());

  auto loadStatData = [&json](const QString& prefix, StatData& obj) {
    obj.maxHitPoints        = json[prefix + "-hp"].toInt();
    obj.armorClass          = json[prefix + "-ac"].toInt();
    obj.actionPoints        = json[prefix + "-ap"].toInt();
    obj.carryWeight         = json[prefix + "-cw"].toInt();
    obj.meleeDamage         = json[prefix + "-md"].toInt();
    obj.damageResistance    = json[prefix + "-dr"].toInt();
    obj.poisonResistance    = json[prefix + "-pr"].toInt();
    obj.radiationResistance = json[prefix + "-rr"].toInt();
    obj.sequence            = json[prefix + "-sq"].toInt();
    obj.healingRate         = json[prefix + "-hr"].toInt();
    obj.criticalChance      = json[prefix + "-cc"].toInt();
    obj.skillRate           = json[prefix + "-sr"].toInt();
    obj.perkRate            = json[prefix + "-pkr"].toInt();

    obj.smallGuns           = json[prefix + "-smallGuns"].toInt();
    obj.bigGuns             = json[prefix + "-bigGuns"].toInt();
    obj.energyGuns          = json[prefix + "-energyGuns"].toInt();
    obj.explosives          = json[prefix + "-explosives"].toInt();
    obj.unarmed             = json[prefix + "-unarmed"].toInt();
    obj.lockpick            = json[prefix + "-lockpick"].toInt();
    obj.meleeWeapons        = json[prefix + "-meleeWeapons"].toInt();
    obj.medicine            = json[prefix + "-medicine"].toInt();
    obj.repair              = json[prefix + "-repair"].toInt();
    obj.science             = json[prefix + "-science"].toInt();
    obj.sneak               = json[prefix + "-sneak"].toInt();
    obj.spellcasting        = json[prefix + "-spellcasting"].toInt();
    obj.steal               = json[prefix + "-steal"].toInt();
    obj.barter              = json[prefix + "-barter"].toInt();
    obj.outdoorsman         = json[prefix + "-outdoorsman"].toInt();
    obj.speech              = json[prefix + "-speech"].toInt();
    obj.gambling            = json[prefix + "-gambling"].toInt();
  };

  loadStatData("base", data);
  loadStatData("mod", modifiers);

  for (const QString& race : json["kills"].toObject().keys())
    addKill(race, json["kills"][race].toVariant().toUInt());

  auto faceColorArray = json["face-color"].toArray();
  spriteTheme = json["sprite-theme"].toString();
  faceTheme   = json["face-theme"].toString();
  faceColor.setRed  (faceColorArray.at(0).toInt());
  faceColor.setGreen(faceColorArray.at(1).toInt());
  faceColor.setBlue (faceColorArray.at(2).toInt());
  faceColor.setAlpha(faceColorArray.at(3).toInt());

  faceAccessories.clear();
  for (QJsonValue value : json["face-accessories"].toArray())
    faceAccessories.push_back(value.toString());

  emit statisticsChanged();
  emit specialChanged();
  emit perksChanged();
  emit traitsChanged();
  emit levelChanged();
  emit hitPointsChanged();
  emit nameChanged();
  emit ageChanged();
  emit experienceChanged();
  emit spriteThemeChanged();
  emit faceAccessoriesChanged();
  emit faceThemeChanged();
  emit faceColorChanged();
  emit raceChanged();
  emit genderChanged();
  emit factionChanged();
}

void StatModel::toJson(QJsonObject& json)
{
  json["name"] = name;
  json["age"]  = age;
  json["race"]   = race;
  json["gender"] = gender;
  json["hp"]   = hitPoints;
  json["lvl"]  = level;
  json["xp"]   = experience;
  json["skill-points"] = skillPoints;
  json["available-perks"] = availablePerks;
  json["last-perk"] = lastPerk;
  json["str"] = strength;
  json["end"] = endurance;
  json["per"] = perception;
  json["cha"] = charisma;
  json["int"] = intelligence;
  json["agi"] = agility;
  json["luc"] = luck;
  json["faction"] = faction;
  json["perks"]  = QJsonArray::fromStringList(perks);
  json["traits"] = QJsonArray::fromStringList(traits);

  auto storeStatData = [&json](const QString& prefix, const StatData& obj) {
    json[prefix + "-hp"]  = obj.maxHitPoints;
    json[prefix + "-ac"]  = obj.armorClass;
    json[prefix + "-ap"]  = obj.actionPoints;
    json[prefix + "-cw"]  = obj.carryWeight;
    json[prefix + "-md"]  = obj.meleeDamage;
    json[prefix + "-dr"]  = obj.damageResistance;
    json[prefix + "-pr"]  = obj.poisonResistance;
    json[prefix + "-rr"]  = obj.radiationResistance;
    json[prefix + "-sq"]  = obj.sequence;
    json[prefix + "-hr"]  = obj.healingRate;
    json[prefix + "-cc"]  = obj.criticalChance;
    json[prefix + "-sr"]  = obj.skillRate;
    json[prefix + "-pkr"] = obj.perkRate;

    json[prefix + "-smallGuns"]    = obj.smallGuns;
    json[prefix + "-bigGuns"]      = obj.bigGuns;
    json[prefix + "-energyGuns"]   = obj.energyGuns;
    json[prefix + "-explosives"]   = obj.explosives;
    json[prefix + "-unarmed"]      = obj.unarmed;
    json[prefix + "-lockpick"]     = obj.lockpick;
    json[prefix + "-meleeWeapons"] = obj.meleeWeapons;
    json[prefix + "-medicine"]     = obj.medicine;
    json[prefix + "-repair"]       = obj.repair;
    json[prefix + "-science"]      = obj.science;
    json[prefix + "-sneak"]        = obj.sneak;
    json[prefix + "-spellcasting"] = obj.spellcasting;
    json[prefix + "-steal"]        = obj.steal;
    json[prefix + "-barter"]       = obj.barter;
    json[prefix + "-outdoorsman"]  = obj.outdoorsman;
    json[prefix + "-speech"]       = obj.speech;
    json[prefix + "-gambling"]     = obj.gambling;
  };

  storeStatData("base", data);
  storeStatData("mod", modifiers);

  QJsonObject killData;
  for (const QString& killedRace : getKilledRaces())
    killData.insert(killedRace, QJsonValue::fromVariant(getKillCount(killedRace)));
  json.insert("kills", killData);

  json["sprite-theme"]     = spriteTheme;
  json["face-theme"]       = faceTheme;
  json["face-color"]       = QJsonArray::fromVariantList(QVariantList() << faceColor.red() << faceColor.green() << faceColor.blue() << faceColor.alpha());
  json["face-accessories"] =  QJsonArray::fromStringList(faceAccessories);
}
