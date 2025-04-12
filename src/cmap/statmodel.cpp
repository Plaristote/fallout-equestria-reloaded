#include "statmodel.h"
#include "i18n.h"
#include <QJsonArray>
#include "cmap/race.h"
#include "cmap/perk.h"
#include "game.h"
#include <QDebug>

#define SKILL(NAME) QPair<QString, StatModel::Skill>(#NAME, {&StatModel::NAME##Increase, &StatModel::NAME##Decrease, &StatModel::NAME##CanDecrease})

const MapWithOrder<QString, StatModel::Skill> StatModel::skillMap = MapWithOrder<QString, StatModel::Skill>()
 << SKILL(smallGuns)
 << SKILL(bigGuns)
 << SKILL(energyGuns)
 << SKILL(explosives)
 << SKILL(unarmed)
 << SKILL(lockpick)
 << SKILL(meleeWeapons)
 << SKILL(medicine)
 << SKILL(repair)
 << SKILL(science)
 << SKILL(sneak)
 << SKILL(spellcasting)
 << SKILL(steal)
 << SKILL(barter)
 << SKILL(outdoorsman)
 << SKILL(speech)
 << SKILL(gambling);

StatModel::StatModel(QObject *parent) : QObject(parent)
{
  strength = perception = endurance = charisma = intelligence = agility = luck = 5;
  hitPoints = 1;
  skillPoints = 0;
  experience = 0;
  faceColor = eyeColor = hairColor = Qt::transparent;
  connect(this, &StatModel::ageChanged,     this, &StatModel::ageClassChanged);
  connect(this, &StatModel::raceChanged,    this, &StatModel::ageClassChanged);
  connect(this, &StatModel::specialChanged, this, &StatModel::updateBaseValues);
  connect(this, &StatModel::traitsChanged,  this, &StatModel::updateBaseValues);
  connect(this, &StatModel::perksChanged,   this, &StatModel::updateBaseValues);
  connect(this, &StatModel::raceChanged,    this, &StatModel::updateBaseValues);
  connect(this, &StatModel::specialChanged, this, &StatModel::acceptableChanged);
  connect(this, &StatModel::traitsChanged,  this, &StatModel::acceptableChanged);
  connect(this, &StatModel::nameChanged,    this, &StatModel::acceptableChanged);
}

float StatModel::hpPercentage() const
{
  float maxHp = static_cast<float>(get_maxHitPoints());

  if (maxHp > 0)
    return static_cast<float>(hitPoints) / maxHp * 100.f;
  return 0;
}

QStringList StatModel::getAvailableRaces() const
{
  Game* game = Game::get();

  if (!game->getIsGameEditor())
  {
    QStringList races;
    const auto& allRaces = Race::getRaces();

    for (const QString& race : allRaces.keys())
    {
      if (allRaces[race].isPlayable())
        races << race;
    }
    return races;
  }
  return Race::getRaces().keys();
}

QStringList StatModel::getAvailableRacesLabels() const
{
  QStringList retval;
  I18n* i18n = I18n::get();

  for (const auto& race : getAvailableRaces())
    retval << i18n->t("races." + race);
  return retval;
}

QStringList StatModel::getGenders() const
{
  return QStringList() << "male" << "female";
}

QStringList StatModel::getGendersLabels() const
{
  I18n* i18n = I18n::get();
  return QStringList() << i18n->t("genders.male") << i18n->t("genders.female");
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
  if (experience + xp > getXpNextLevel())
  {
    do
    {
      xp -= getXpNextLevel() - experience;
      experience = getXpNextLevel();
      levelUp();
    } while (experience + xp >= getXpNextLevel());
  }
  else
    experience += xp;
  emit experienceChanged();
}

void StatModel::levelUp()
{
  hasLeveledUp = true;
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
  emit leveledUpChanged();
}

QVariantMap StatModel::getAgeRange() const
{
  const Race*    raceController = getRaceController();
  Race::AgeRange ageRange;

  if (raceController)
    ageRange = raceController->getAgeRange();
  return ageRange.toVariant();
}

QString StatModel::getAgeClass() const
{
  const Race*    raceController = getRaceController();
  Race::AgeRange ageRange;

  if (raceController)
    ageRange = raceController->getAgeRange();
  if (age < ageRange.adult)
    return "child";
  if (age > ageRange.old)
    return "old";
  return "adult";
}

const Race* StatModel::getRaceController() const
{
  const auto& races = Race::getRaces();
  auto it = races.find(race);

  if (it != races.end())
    return &(*it);
  return nullptr;
}

void StatModel::setRace(const QString& newRace)
{
  const Race* raceController = getRaceController();

  if (raceController)
    raceController->toogle(this, false);
  race = newRace;
  raceController = getRaceController();
  if (raceController)
    raceController->toogle(this, true);
  race = newRace;
  if (raceController && !raceController->withFaceColor())
    faceColor = hairColor = eyeColor = Qt::transparent;
  emit raceChanged();
  updateBaseValues();
}

QStringList StatModel::getAvailableFaces() const
{
  const Race* raceController = getRaceController();

  if (raceController)
    return raceController->getFaces();
  return QStringList();
}

QStringList StatModel::getAvailableHairs() const
{
  const Race* raceController = getRaceController();

  if (raceController)
    return raceController->getHairs(faceTheme);
  return QStringList();
}

bool StatModel::withFaceColor() const
{
  const Race* raceController = getRaceController();

  if (raceController)
    return raceController->withFaceColor();
  return false;
}

void StatModel::addProficiency(const QString &skillName)
{
  if (!proficiencies.contains(skillName))
  {
    proficiencies << skillName;
    emit proficienciesChanged();
  }
}

void StatModel::removeProficiency(const QString &skillName)
{
  if (proficiencies.contains(skillName))
  {
    proficiencies.removeOne(skillName);
    emit proficienciesChanged();
  }
}

void StatModel::toggleProficiency(const QString &skillName)
{
  if (proficiencies.contains(skillName))
    removeProficiency(skillName);
  else
    addProficiency(skillName);
}

int StatModel::skillIncreaseCost(const QString& skillName) const
{
  int value = property(skillName.toStdString().c_str()).toInt();

  if (value > 100)
    return 2;
  return 1;
}

void StatModel::increaseSkill(const QString& skillName, int& skillValue, int& spentPoints)
{
  int amount = proficiencies.contains(skillName) ? 2 : 1;

  skillValue  += amount;
  spentPoints += amount;
  skillPoints -= skillIncreaseCost(skillName);
  emit skillPointsChanged();
  emit statisticsChanged();
}

void StatModel::decreaseSkill(const QString &skillName, int &skillValue, int& spentPoints)
{
  int amount = proficiencies.contains(skillName) ? 2 : 1;

  skillPoints += skillIncreaseCost(skillName);
  spentPoints -= amount;
  skillValue  -= amount;
  emit skillPointsChanged();
  emit statisticsChanged();
}

static void setSkillValue(SkillData& data, const QString& skillName, int value)
{
  auto i = StatModel::skillMap.keys().indexOf(skillName);

  if (i >= 0)
  {
    int* values = reinterpret_cast<int*>(&data);

    values[i] = value;
  }
  else
    qDebug() << "StatModel::setSkillValue: skill" << skillName << "not found.";
}

static int getSkillValue(const SkillData& data, const QString& skillName)
{
  auto i = StatModel::skillMap.keys().indexOf(skillName);

  if (i >= 0)
    return reinterpret_cast<const int*>(&data)[i];
  else
    qDebug() << "StatModel::getSkillValue: skill" << skillName << "not found.";
  return 0;
}

static void applyCmapPlugin(StatModel* self, const CmapPlugin& plugin, StatData& data)
{
  data.actionPoints        = plugin.modifyBaseStatistic(self, "actionPoints",        data.actionPoints);
  data.armorClass          = plugin.modifyBaseStatistic(self, "armorClass",          data.armorClass);
  data.carryWeight         = plugin.modifyBaseStatistic(self, "carryWeight",         data.carryWeight);
  data.criticalChance      = plugin.modifyBaseStatistic(self, "criticalChance",      data.criticalChance);
  data.damageResistance    = plugin.modifyBaseStatistic(self, "damageResistance",    data.damageResistance);
  data.healingRate         = plugin.modifyBaseStatistic(self, "healingRate",         data.healingRate);
  data.maxHitPoints        = plugin.modifyBaseStatistic(self, "maxHitPoints",        data.maxHitPoints);
  data.meleeDamage         = plugin.modifyBaseStatistic(self, "meleeDamage",         data.meleeDamage);
  data.perkRate            = plugin.modifyBaseStatistic(self, "perkRate",            data.perkRate);
  data.poisonResistance    = plugin.modifyBaseStatistic(self, "poisonResistance",    data.poisonResistance);
  data.radiationResistance = plugin.modifyBaseStatistic(self, "radiationResistance", data.radiationResistance);
  data.skillRate           = plugin.modifyBaseStatistic(self, "skillRate",           data.skillRate);

  for (auto it = StatModel::skillMap.begin() ; it != StatModel::skillMap.end() ; ++it)
  {
    int value = getSkillValue(data, it.key());

    value = plugin.modifyBaseSkill(self, it.key(), value);
    setSkillValue(data, it.key(), value);
  }
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
  data.sequence            = 2 * perception;

  data.smallGuns    = 5 + 4 * agility;
  data.bigGuns      = strength + 2 * agility;
  data.energyGuns   = intelligence + 2 * agility;
  data.explosives   = 10 + perception + agility;
  data.unarmed      = 30 + (2 * (agility + strength));
  data.meleeWeapons = 20 + (2 * (agility + strength));
  data.lockpick     = 15 + perception + agility;
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

  const auto& allTraits      = Trait::getTraits();
  const auto& allPerks       = Perk::getPerks();
  auto*       raceController = getRaceController();

  for (auto trait : allTraits)
  {
    if (traits.contains(trait.name))
      applyCmapPlugin(this, trait, data);
  }
  for (auto perk : allPerks)
  {
    for (int i = perks.count(perk.name) ; i > 0 ; --i)
      applyCmapPlugin(this, perk, data);
  }
  if (raceController)
    applyCmapPlugin(this, *raceController, data);
  else
    qDebug() << "/!\\ Missing race controller for race" << race;
  emit statisticsChanged();
}

bool StatModel::isAcceptable() const
{
  return traits.length() <= getMaxTraits() && specialPoints == 0 && name.length() > 0;
}

QStringList StatModel::getAvailablePerks()
{
  return Perk::getAvailablePerks(this);
}

QStringList StatModel::getAvailableTraits()
{
  QStringList results;
  const auto& traits = Trait::getTraits();

  for (const auto& trait : traits)
    results << trait.name;
  return results;
}

QStringList StatModel::getAvailableTraitsLabels()
{
  QStringList retval;
  I18n* i18n = I18n::get();
  const auto list = getAvailableTraits();

  for (const auto& trait : list)
    retval << i18n->t("cmap." + trait);
  return retval;
}

void StatModel::toggleTrait(const QString& name, bool value)
{
  const auto& traits = Trait::getTraits();

  if ((value  && this->traits.contains(name)) ||
      (!value && !this->traits.contains(name)))
    return ;
  for (const auto& trait : traits)
  {
    if  (trait.name == name)
    {
      trait.toogle(this, value);
      if  (value)
        this->traits << trait.name;
      else
        this->traits.removeAll(trait.name);
      emit traitsChanged();
      break ;
    }
  }
}

void StatModel::togglePerk(const QString &name, bool value)
{
  const auto& perks = Perk::getPerks();

  for (const auto& perk : perks)
  {
    if (perk.name == name)
    {
      perk.toogle(this, value);
      if (value)
        this->perks << perk.name;
      else
        this->perks.removeOne(perk.name);
      availablePerks = availablePerks + (value ? -1 : 1);
      emit perksChanged();
      emit availablePerksChanged();
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
  for (auto it = skillMap.begin() ; it != skillMap.end() ; ++it)
  {
    while ((this->*(it->canDecrease))())
      (this->*(it->decrease))();
  }
  emit statisticsChanged();
  emit skillPointsChanged();
}

QStringList StatModel::getSkillList()
{
  return skillMap.keys();
}

QColor jsonToColor(QJsonValue value)
{
  QColor result = Qt::transparent;

  if (value.isArray())
  {
    auto array = value.toArray();

    result.setRed   (array.at(0).toInt());
    result.setGreen(array.at(1).toInt());
    result.setBlue (array.at(2).toInt());
    result.setAlpha(array.at(3).toInt());
  }
  return result;
}

QJsonArray colorToJson(QColor color)
{
  QVariantList list;

  list << color.red() << color.green() << color.blue() << color.alpha();
  return QJsonArray::fromVariantList(list);
}

void StatModel::fromTemplate(const QString &name)
{
  QFile file(ASSETS_PATH + "characterTemplates/" + name + ".json");

  if (file.open(QIODevice::ReadOnly))
  {
    QByteArray json = file.readAll();

    file.close();
    fromJson(QJsonDocument::fromJson(json).object());
    setProperty("specialPoints", 0);
  }
  else
  {
    qDebug() << "StatModel::fromTemplate: template" << name << "not found";
    fromJson(QJsonObject());
    setProperty("specialPoints", 5);
  }
  setHitPoints(get_maxHitPoints());
}

void StatModel::fromJson(const QJsonObject& json)
{
  const QJsonArray jsonTraits        = json["traits"].toArray();
  const QJsonArray jsonPerks         = json["perks"].toArray();
  const QJsonArray jsonProficiencies = json["profs"].toArray();
  const QJsonArray jsonSpells        = json["spells"].toArray();

  name = json["name"].toString();
  age  = json["age"].toInt(21);
  race           = json["race"].toString();
  gender         = json["gender"].toString();
  hitPoints      = json["hp"].toInt(1);
  level          = json["lvl"].toInt(1);
  experience     = json["xp"].toInt();
  hasLeveledUp   = json["lvlup"].toBool(false);
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
  maxProficiencies = json["mp"].toInt(3);
  traits.clear();
  perks.clear();
  proficiencies.clear();
  for (const QJsonValue& value : jsonTraits)
    traits.push_back(value.toString());
  for (const QJsonValue& value : jsonPerks)
    perks.push_back(value.toString());
  for (const QJsonValue& value : jsonProficiencies)
    proficiencies.push_back(value.toString());
  for (const QJsonValue& value : jsonSpells)
    spells.push_back(value.toString());

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

    for (auto it = skillMap.begin() ; it != skillMap.end() ; ++it)
      setSkillValue(obj, it.key(), json[prefix + '-' + it.key()].toInt());
  };

  loadStatData("base", data);
  loadStatData("mod", modifiers);

  for (const QString& race : json["kills"].toObject().keys())
    addKill(race, json["kills"][race].toVariant().toUInt());

  spriteTheme = json["sprite-theme"].toString();
  faceTheme   = json["face-theme"].toString();
  hairTheme   = json["hair-theme"].toString();
  faceColor   = jsonToColor(json["face-color"]);
  eyeColor    = jsonToColor(json["eye-color"]);
  hairColor   = jsonToColor(json["hair-color"]);

  faceAccessories.clear();
  for (QJsonValue value : json["face-accessories"].toArray())
    faceAccessories.push_back(value.toString());

  emit statisticsChanged();
  emit specialChanged();
  emit perksChanged();
  emit traitsChanged();
  emit buffsChanged();
  emit levelChanged();
  emit hitPointsChanged();
  emit nameChanged();
  emit ageChanged();
  emit experienceChanged();
  emit spriteThemeChanged();
  emit faceAccessoriesChanged();
  emit faceThemeChanged();
  emit hairThemeChanged();
  emit faceColorChanged();
  emit eyeColorChanged();
  emit hairColorChanged();
  emit raceChanged();
  emit genderChanged();
  emit factionChanged();
  emit proficienciesChanged();
  emit spellsChanged();
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
  if (hasLeveledUp)
    json["lvlup"] = true;
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
  json["profs"]  = QJsonArray::fromStringList(proficiencies);
  json["spells"] = QJsonArray::fromStringList(spells);
  json["mp"] = maxProficiencies;

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

    for (auto it = skillMap.begin() ; it != skillMap.end() ; ++it)
      json[prefix + '-' + it.key()] = getSkillValue(obj, it.key());
  };

  storeStatData("base", data);
  storeStatData("mod", modifiers);

  QJsonObject killData;
  for (const QString& killedRace : getKilledRaces())
    killData.insert(killedRace, QJsonValue::fromVariant(getKillCount(killedRace)));
  json.insert("kills", killData);

  json["sprite-theme"]     = spriteTheme;
  json["face-theme"]       = faceTheme;
  json["hair-theme"]       = hairTheme;
  json["face-color"]       = colorToJson(faceColor);
  json["eye-color"]        = colorToJson(eyeColor);
  json["hair-color"]       = colorToJson(hairColor);
  json["face-accessories"] =  QJsonArray::fromStringList(faceAccessories);
}
