#ifndef STATMODEL_H
# define STATMODEL_H

# include <QObject>
# include <QColor>
# include <QMap>
# include <QJsonDocument>
# include <QJsonObject>

class Race;

struct SkillData
{
  int smallGuns = 0;
  int bigGuns = 0;
  int energyGuns = 0;
  int explosives = 0;
  int unarmed = 0;
  int lockpick = 0;
  int meleeWeapons = 0;
  int medicine = 0;
  int repair = 0;
  int science = 0;
  int sneak = 0;
  int spellcasting = 0;
  int steal = 0;
  int barter = 0;
  int outdoorsman = 0;
  int speech = 0;
  int gambling = 0;
};

struct StatData : public SkillData
{
  int maxHitPoints = 0;
  int armorClass = 0;
  int actionPoints = 0;
  int carryWeight = 0;
  int meleeDamage = 0;
  int damageResistance = 0;
  int poisonResistance = 0;
  int radiationResistance = 0;
  int sequence = 0;
  int healingRate = 0;
  int criticalChance = 0;
  int skillRate = 0;
  int perkRate = 0;
};

class StatModel : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name    MEMBER name    NOTIFY nameChanged)
  Q_PROPERTY(int     age     MEMBER age     NOTIFY ageChanged)
  Q_PROPERTY(QString faction MEMBER faction NOTIFY factionChanged)
  Q_PROPERTY(QString race    READ getRace WRITE setRace NOTIFY raceChanged)
  Q_PROPERTY(QString gender  MEMBER gender  NOTIFY genderChanged)

  Q_PROPERTY(int  hitPoints      MEMBER hitPoints    NOTIFY hitPointsChanged)
  Q_PROPERTY(int  experience     READ getExperience  NOTIFY experienceChanged)
  Q_PROPERTY(int  level          MEMBER level        NOTIFY levelChanged)
  Q_PROPERTY(int  xpNextLevel    READ getXpNextLevel NOTIFY levelChanged)
  Q_PROPERTY(int  skillPoints    MEMBER skillPoints  NOTIFY skillPointsChanged)
  Q_PROPERTY(int  availablePerks MEMBER availablePerks NOTIFY availablePerksChanged)
  Q_PROPERTY(int  specialPoints  MEMBER specialPoints NOTIFY specialChanged)
  Q_PROPERTY(bool acceptable     READ isAcceptable NOTIFY acceptableChanged)

  Q_PROPERTY(QJsonDocument variables MEMBER variables)

  // SPECIAL
  Q_PROPERTY(int strength     MEMBER strength     NOTIFY specialChanged)
  Q_PROPERTY(int perception   MEMBER perception   NOTIFY specialChanged)
  Q_PROPERTY(int endurance    MEMBER endurance    NOTIFY specialChanged)
  Q_PROPERTY(int charisma     MEMBER charisma     NOTIFY specialChanged)
  Q_PROPERTY(int intelligence MEMBER intelligence NOTIFY specialChanged)
  Q_PROPERTY(int agility      MEMBER agility      NOTIFY specialChanged)
  Q_PROPERTY(int luck         MEMBER luck         NOTIFY specialChanged)

  Q_PROPERTY(QStringList perks  MEMBER perks  NOTIFY perksChanged)
  Q_PROPERTY(QStringList traits MEMBER traits NOTIFY traitsChanged)
  Q_PROPERTY(QStringList buffs  MEMBER buffs  NOTIFY buffsChanged)

  // Statistics
  Q_PROPERTY(int maxHitPoints        READ get_maxHitPoints WRITE set_maxHitPoints NOTIFY statisticsChanged)
  Q_PROPERTY(int armorClass          READ get_armorClass WRITE set_armorClass NOTIFY statisticsChanged)
  Q_PROPERTY(int actionPoints        READ get_actionPoints WRITE set_actionPoints NOTIFY statisticsChanged)
  Q_PROPERTY(int carryWeight         READ get_carryWeight WRITE set_carryWeight NOTIFY statisticsChanged)
  Q_PROPERTY(int meleeDamage         READ get_meleeDamage WRITE set_meleeDamage NOTIFY statisticsChanged)
  Q_PROPERTY(int damageResistance    READ get_damageResistance WRITE set_damageResistance NOTIFY statisticsChanged)
  Q_PROPERTY(int poisonResistance    READ get_poisonResistance WRITE set_poisonResistance NOTIFY statisticsChanged)
  Q_PROPERTY(int radiationResistance READ get_radiationResistance WRITE set_radiationResistance NOTIFY statisticsChanged)
  Q_PROPERTY(int sequence            READ get_sequence WRITE set_sequence NOTIFY statisticsChanged)
  Q_PROPERTY(int healingRate         READ get_healingRate WRITE set_healingRate NOTIFY statisticsChanged)
  Q_PROPERTY(int criticalChance      READ get_criticalChance WRITE set_criticalChance NOTIFY statisticsChanged)
  Q_PROPERTY(int skillRate           READ get_skillRate WRITE set_skillRate NOTIFY statisticsChanged)
  Q_PROPERTY(int perkRate            READ get_perkRate WRITE set_perkRate NOTIFY statisticsChanged)

  // Skills
  Q_PROPERTY(int smallGuns    READ get_smallGuns    WRITE set_smallGuns    NOTIFY statisticsChanged)
  Q_PROPERTY(int bigGuns      READ get_bigGuns      WRITE set_bigGuns      NOTIFY statisticsChanged)
  Q_PROPERTY(int energyGuns   READ get_energyGuns   WRITE set_energyGuns   NOTIFY statisticsChanged)
  Q_PROPERTY(int explosives   READ get_explosives   WRITE set_explosives   NOTIFY statisticsChanged)
  Q_PROPERTY(int unarmed      READ get_unarmed      WRITE set_unarmed      NOTIFY statisticsChanged)
  Q_PROPERTY(int meleeWeapons READ get_meleeWeapons WRITE set_meleeWeapons NOTIFY statisticsChanged)
  Q_PROPERTY(int lockpick     READ get_lockpick     WRITE set_lockpick     NOTIFY statisticsChanged)
  Q_PROPERTY(int medicine     READ get_medicine     WRITE set_medicine     NOTIFY statisticsChanged)
  Q_PROPERTY(int repair       READ get_repair       WRITE set_repair       NOTIFY statisticsChanged)
  Q_PROPERTY(int science      READ get_science      WRITE set_science      NOTIFY statisticsChanged)
  Q_PROPERTY(int sneak        READ get_sneak        WRITE set_sneak        NOTIFY statisticsChanged)
  Q_PROPERTY(int spellcasting READ get_spellcasting WRITE set_spellcasting NOTIFY statisticsChanged)
  Q_PROPERTY(int steal        READ get_steal        WRITE set_steal        NOTIFY statisticsChanged)
  Q_PROPERTY(int barter       READ get_barter       WRITE set_barter       NOTIFY statisticsChanged)
  Q_PROPERTY(int outdoorsman  READ get_outdoorsman  WRITE set_outdoorsman  NOTIFY statisticsChanged)
  Q_PROPERTY(int speech       READ get_speech       WRITE set_speech       NOTIFY statisticsChanged)
  Q_PROPERTY(int gambling     READ get_gambling     WRITE set_gambling     NOTIFY statisticsChanged)

  // VISUAL
  Q_PROPERTY(QStringList faceOptions     READ   getAvailableFaces NOTIFY raceChanged)
  Q_PROPERTY(QStringList hairOptions     READ   getAvailableHairs NOTIFY faceThemeChanged)
  Q_PROPERTY(QStringList faceAccessories MEMBER faceAccessories   NOTIFY faceAccessoriesChanged)
  Q_PROPERTY(QString     spriteTheme     MEMBER spriteTheme       NOTIFY spriteThemeChanged)
  Q_PROPERTY(QString     faceTheme       MEMBER faceTheme         NOTIFY faceThemeChanged)
  Q_PROPERTY(QString     hairTheme       MEMBER hairTheme         NOTIFY hairThemeChanged)
  Q_PROPERTY(bool        withFaceColor   READ   withFaceColor     NOTIFY raceChanged)
  Q_PROPERTY(QColor      faceColor       MEMBER faceColor         NOTIFY faceColorChanged)
  Q_PROPERTY(QColor      eyeColor        MEMBER eyeColor          NOTIFY eyeColorChanged)
  Q_PROPERTY(QColor      hairColor       MEMBER hairColor         NOTIFY hairColorChanged)

public:
  explicit StatModel(QObject *parent = nullptr);

  void fromJson(const QJsonObject&);
  void toJson(QJsonObject&);

  int getXpNextLevel() const;
  void levelUp();

  QStringList& rbuffs() { return buffs; }

  const QString& getRace() const { return race; }
  void setRace(const QString& newRace);
  const QString& getName() const { return name; }
  int getExperience() const { return experience; }
  int getHitPoints() const { return hitPoints; }
  void setHitPoints(int value) { hitPoints = value; emit hitPointsChanged(); }
  QString getFaction() const { return faction; }
  QStringList getAvailableFaces() const;
  QStringList getAvailableHairs() const;
  bool withFaceColor() const;
  Q_INVOKABLE void addExperience(int xp);
  Q_INVOKABLE bool isAcceptable() const;
  Q_INVOKABLE int getMaxTraits() const { return 2; }
  Q_INVOKABLE QStringList getAvailableTraits();
  Q_INVOKABLE QStringList getAvailableTraitsLabels();
  Q_INVOKABLE QStringList getAvailablePerks();
  Q_INVOKABLE void toggleTrait(const QString& name, bool);
  Q_INVOKABLE void togglePerk(const QString& name, bool);
  Q_INVOKABLE void setFaceColor(int r, int g, int b, int a) { faceColor = QColor(r, g, b, a); emit faceColorChanged(); }
  Q_INVOKABLE void setEyeColor(int r, int g, int b, int a)  { eyeColor  = QColor(r, g, b, a); emit eyeColorChanged(); }
  Q_INVOKABLE void setHairColor(int r, int g, int b, int a) { hairColor = QColor(r, g, b, a); emit hairColorChanged(); }
  Q_INVOKABLE void toggleFaceAccessory(const QString&);

  Q_INVOKABLE void confirmChanges();
  Q_INVOKABLE void cancelChanges();

  const Race* getRaceController() const;

#define STAT_METHODS(statName) \
  int get_##statName() const { return data.statName + modifiers.statName; } \
  void  set_##statName(int value) { modifiers.statName = value - data.statName; emit statisticsChanged(); }

#define SKILL_METHODS(skillName) \
  STAT_METHODS(skillName) \
  Q_INVOKABLE bool skillName##CanDecrease() { return spentPoints.skillName > 0; } \
  Q_INVOKABLE void skillName##Increase() { spentPoints.skillName++; modifiers.skillName++; skillPoints--; emit skillPointsChanged(); emit statisticsChanged(); } \
  Q_INVOKABLE void skillName##Decrease() { spentPoints.skillName--; modifiers.skillName--; skillPoints++; emit skillPointsChanged(); emit statisticsChanged(); }

  // Statistics
  STAT_METHODS(maxHitPoints)
  STAT_METHODS(armorClass)
  STAT_METHODS(actionPoints)
  STAT_METHODS(carryWeight)
  STAT_METHODS(meleeDamage)
  STAT_METHODS(damageResistance)
  STAT_METHODS(poisonResistance)
  STAT_METHODS(radiationResistance)
  STAT_METHODS(sequence)
  STAT_METHODS(healingRate)
  STAT_METHODS(criticalChance)
  STAT_METHODS(skillRate)
  STAT_METHODS(perkRate)
  // Skills
  SKILL_METHODS(smallGuns)
  SKILL_METHODS(bigGuns)
  SKILL_METHODS(energyGuns)
  SKILL_METHODS(explosives)
  SKILL_METHODS(unarmed)
  SKILL_METHODS(lockpick)
  SKILL_METHODS(meleeWeapons)
  SKILL_METHODS(medicine)
  SKILL_METHODS(repair)
  SKILL_METHODS(science)
  SKILL_METHODS(sneak)
  SKILL_METHODS(spellcasting)
  SKILL_METHODS(steal)
  SKILL_METHODS(barter)
  SKILL_METHODS(outdoorsman)
  SKILL_METHODS(speech)
  SKILL_METHODS(gambling)

  Q_INVOKABLE QStringList  getKilledRaces() const { return kills.keys(); }
  Q_INVOKABLE unsigned int getKillCount(const QString& race) const { return kills.contains(race) ? kills[race] : 0; }
  Q_INVOKABLE void         addKill(const QString& race, unsigned int amount = 1);

  // EDITOR
  Q_INVOKABLE QStringList getAvailableRaces() const;
  Q_INVOKABLE QStringList getAvailableRacesLabels() const;
  Q_INVOKABLE QStringList getGenders() const;
  Q_INVOKABLE QStringList getGendersLabels() const;

signals:
  void damageReceived(int damage, QString type);
  void nameChanged();
  void ageChanged();
  void hitPointsChanged();
  void experienceChanged();
  void levelChanged();
  void availablePerksChanged();
  void skillPointsChanged();
  void specialChanged();
  void statisticsChanged();
  void perksChanged();
  void traitsChanged();
  void buffsChanged();
  void acceptableChanged();
  void spriteThemeChanged();
  void faceAccessoriesChanged();
  void faceThemeChanged();
  void hairThemeChanged();
  void faceColorChanged();
  void eyeColorChanged();
  void hairColorChanged();
  void factionChanged();
  void raceChanged();
  void genderChanged();

private slots:
  void updateBaseValues();

private:
  QString name, faction;
  int age = 21;
  QString gender;
  QString race;
  int strength, perception, endurance, charisma, intelligence, agility, luck;
  int hitPoints = 1;
  int skillPoints = 0;
  int availablePerks = 0;
  int lastPerk = 0;
  int level = 1;
  int experience = 0;
  int specialPoints = 0;

  StatData  data;
  StatData  modifiers;
  SkillData spentPoints;

  QStringList traits, perks, buffs;
  QJsonDocument variables;
  QMap<QString, unsigned int> kills;

  QString     spriteTheme, faceTheme, hairTheme;
  QColor      faceColor, eyeColor, hairColor;
  QStringList faceAccessories;
};

#endif // STATMODEL_H
