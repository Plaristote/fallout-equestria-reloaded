#include "characterparty.h"
#include "leveltask.h"
#include <QJsonArray>
#include <QDebug>
#include <algorithm>

const QString originalFactionVarname("_originalFaction");

static bool findWithCallback(Character* character, QJSValue callback)
{
  return callback.call(QJSValueList() << character->asJSValue()).toBool();
}

static bool findByObjectName(Character* character, const QString& name)
{
  return character->getObjectName() == name;
}

CharacterParty::CharacterParty(QObject *parent) : QObject(parent)
{
  connect(this, &CharacterParty::factionNameChanged, this, &CharacterParty::updateFaction);
}

static Character* variantToCharacter(const QVariant& value)
{
  return value.typeId() == QMetaType::QObjectStar
    ? qobject_cast<Character*>(qvariant_cast<QObject*>(value))
    : nullptr;
}

CharacterParty* CharacterParty::factory(const QVariantMap& parameters, QObject* parent)
{
  const QVariantList members(parameters["members"].toList());
  CharacterParty*    party = new CharacterParty(parent);

  qDebug() << "CharacterParty::factory" << parameters;
  party->setProperty("name", parameters["name"]);
  if (!parameters["faction"].isNull())
    party->setProperty("factionName", parameters["faction"].toString());
  for (const QVariant& entry : members)
  {
    Character* character = variantToCharacter(entry);

    if (character)
      party->addCharacter(character);
    else if (entry.typeId() == QMetaType::QVariantMap)
      party->createCharacters(entry.toMap());
    else
      qDebug() << "CharacterParty::factory: invalid member value" << entry;
  }
  return party;
}

void CharacterParty::createCharacters(const QVariantMap& characterData)
{
  unsigned int count = characterData.value("amount", 1).toUInt();

  if (characterData.contains("sheet"))
  {
    for (unsigned int i = 0 ; i < count ; ++i)
    {
      const QString defaultName = getName() + '#' + QString::number(list.length());
      const QString name = characterData.value("name", defaultName).toString();

      createCharacter(name, characterData);
    }
  }
  else
    qDebug() << "CharacterParty::createCharacters: missing `sheet` property for character";
}

void CharacterParty::createCharacter(const QString& name, const QVariantMap& attributes)
{
  Character* character = new Character(this);
  Inventory* inventory = character->getInventory();

  character->setObjectName(name);
  character->setUnique(attributes.value("uniq", false).toBool());
  character->setCharacterSheet(attributes["sheet"].toString());
  character->setScript(attributes.value("script", "character.mjs").toString());
  if (!attributes.value("inventory").isNull())
    inventory->load(QJsonObject::fromVariantMap(attributes.value("inventory").toMap()));
  character->updateSpriteSheet();
  if (factionName.isEmpty())
    useCharacterFaction(character);
  addCharacter(character);
}

void CharacterParty::useCharacterFaction(const Character* character)
{
  if (character)
  {
    QString newFaction = character->getStatistics()->getFaction();

    if (newFaction != factionName)
      setFactionName(newFaction);
  }
}

void CharacterParty::updateFaction()
{
  for (Character* character : list)
    enforceFactionOn(character);
}

void CharacterParty::enforceFactionOn(Character* character)
{
  QString originalFaction = character->getStatistics()->getFaction();

  if (originalFaction != factionName)
  {
    character->getStatistics()->setProperty("faction", factionName);
    if (!character->hasVariable(originalFactionVarname))
      character->setVariable(originalFactionVarname, originalFaction);
  }
}

void CharacterParty::rollbackFactionOn(Character* character)
{
  if (character->hasVariable(originalFactionVarname))
  {
    character->getStatistics()->setProperty("faction", character->getVariable(originalFactionVarname));
    character->unsetVariable(originalFactionVarname);
  }
}

void CharacterParty::addCharacter(Character* character)
{
  if (character)
  {
    connect(character, &DynamicObject::beforeDestroy, this, [this, character]() { removeCharacter(character); });
    connect(character, &Character::joinedCombat, this, &CharacterParty::joinCombat);
    character->setParent(this);
    enforceFactionOn(character);
    list.push_back(character);
    emit partyChanged();
  }
}

void CharacterParty::removeCharacter(Character* character)
{
  if (character)
  {
    disconnect(character, &Character::joinedCombat, this, &CharacterParty::joinCombat);
    list.removeAll(character);
    rollbackFactionOn(character);
    emit partyChanged();
    qDebug() << "removing character vrom party" << character->getDisplayName() << "(remaining characters" << list.length() << ')';
  }
}

bool CharacterParty::containsCharacter(const Character* character) const
{
  return list.count(const_cast<Character*>(character)) > 0;
}

Character* CharacterParty::find(QJSValue callback) const
{
  std::function<bool (Character*)> predicate = [](Character*) { return false; };
  QList<Character*>::const_iterator iterator;

  if (callback.isQObject())
    iterator = std::find(list.begin(), list.end(), callback.toQObject());
  else
  {
    if (callback.isCallable())
      predicate = std::bind(findWithCallback, std::placeholders::_1, callback);
    else if (callback.isString())
      predicate = std::bind(findByObjectName, std::placeholders::_1, callback.toString());
    iterator = std::find_if(list.begin(), list.end(), predicate);
  }
  return iterator != list.end() ? *iterator : nullptr;
}

Character* CharacterParty::mostSkilledAt(const QByteArray& stat) const
{
  Character* result = nullptr;
  int max = 0;

  for (Character* character : list)
  {
    int value = character->getStatistics()->property(stat).toInt();
    if (value > max || !result)
    {
      result = character;
      max = value;
    }
  }
  return result;
}

Character* CharacterParty::leastSkilledAt(const QByteArray& stat) const
{
  Character* result = nullptr;
  int min = 0;

  for (Character* character : list)
  {
    int value = character->getStatistics()->property(stat).toInt();
    if (value < min || !result)
    {
      result = character;
      min = value;
    }
  }
  return result;
}

int CharacterParty::highestStatistic(const QByteArray& name) const
{
  Character* character = mostSkilledAt(name);
  return character ? character->getStatistics()->property(name).toInt() : 0;
}

int CharacterParty::lowestStatistic(const QByteArray& name) const
{
  Character* character = leastSkilledAt(name);
  return character ? character->getStatistics()->property(name).toInt() : 0;
}

Character* CharacterParty::get(const QString& name)
{
  for (auto it = list.begin() ; it != list.end() ; ++it)
  {
    if ((*it)->getStatistics()->getName() == name)
      return *it;
  }
  return nullptr;
}

void CharacterParty::addExperience(unsigned int value)
{
  for (auto* character : std::as_const(list))
    character->getStatistics()->addExperience(static_cast<int>(value));
}

bool CharacterParty::insertIntoZone(GridComponent* level, TileZone* zone) const
{
  auto* grid = level->getFloorGrid(zone->getFloor());
  int characterIt = 0;

  if (list.length() > 0)
  {
    for (auto position : zone->getPositions())
    {
      if (!grid->isOccupied(position.x(), position.y()))
      {
        Character* character = list.at(characterIt);

        level->appendObject(character);
        level->setCharacterPosition(character, position.x(), position.y(), zone->getFloor());
        if (++characterIt >= list.length())
          break ;
      }
    }
    for (int i = 0 ; i < characterIt ; ++i)
      list[i]->scriptCall("insertedIntoZone", QJSValueList() << zone->getName());
  }
  return characterIt >= list.length();
}

bool CharacterParty::insertIntoZone(GridComponent* level, const QString &zoneName) const
{
  auto* tileMap = level->getTileMap();

  for (auto* zone : tileMap->getZones())
  {
    if (zone->getName() == zoneName)
      return insertIntoZone(level, zone);
  }
  return false;
}

bool CharacterParty::insertIntoZone(GridComponent* level, QJSValue param) const
{
  if (param.isString())
    return insertIntoZone(level, param.toString());
  else if (param.isQObject())
    return insertIntoZone(level, reinterpret_cast<TileZone*>(param.toQObject()));
  else
    qDebug() << "CharacterParty::insertIntoZone: called with invalid parameter";
  return false;
}

void CharacterParty::extractFromLevel(GridComponent* level)
{
  auto* grid = level->getGrid();

  for (Character* character : list)
  {
    character->clearCurrentZones();
    character->getFieldOfView()->reset();
    grid->removeObject(character);
    level->detachObject(character);
    character->setParent(this);
  }
}

void CharacterParty::save(QJsonObject& data) const
{
  QJsonArray charactersData;

  data["name"] = name;
  for (Character* character : list)
  {
    QJsonObject characterData;

    character->save(characterData);
    charactersData << characterData;
  }
  data.insert("list", charactersData);
}

void CharacterParty::load(const QJsonObject& data)
{
  name = data["name"].toString();
  for (QJsonValue characterDataV : data["list"].toArray())
  {
    QJsonObject characterData = characterDataV.toObject();
    Character*  character = new Character(this);

    character->load(characterData);
    addCharacter(character);
  }
  if (list.length() == 0)
    qDebug() << "Fatal error: saved data did not contain any player character";
}

void CharacterParty::loadIntoLevel(GridComponent* level)
{
  auto* grid = level->getGrid();

  for (Character* character : list)
  {
    grid->moveObject(character, character->getPosition().x(), character->getPosition().y());
    level->appendObject(character);
  }
}

void CharacterParty::requireJoinCombat()
{
  Character* character = mostSkilledAt("sequence");

  if (character)
    character->requireJoinCombat();
}

void CharacterParty::joinCombat()
{
  CombatComponent* level = LevelTask::get();

  for (Character* character : list)
    level->joinCombat(character);
}
