#include "characterparty.h"
#include "leveltask.h"
#include <QJsonArray>
#include <QDebug>

CharacterParty::CharacterParty(QObject *parent) : QObject(parent)
{

}

CharacterParty* CharacterParty::factory(const QVariantMap& parameters, QObject* parent)
{
  const QVariantList members(parameters["members"].toList());
  CharacterParty*    party = new CharacterParty(parent);
  unsigned int       it = 1;

  party->setProperty("name", parameters["name"]);
  for (const QVariant& entry : members)
  {
    const QVariantMap characterData = entry.toMap();
    unsigned int count = characterData.value("amount", 1).toUInt();

    if (characterData.contains("sheet"))
    {
      for (unsigned int i = 0 ; i < count ; ++i)
      {
        const QString defaultName = party->getName() + '#' + QString::number(it);
        const QString name = characterData.value("name", defaultName).toString();

        party->createCharacter(name, characterData);
        it++;
      }
    }
    else
      qDebug() << "partyFactory: missing `sheet` property for character";
  }
  return party;
}

void CharacterParty::createCharacter(const QString& name, const QVariantMap& attributes)
{
  Character* character = new Character;

  character->setObjectName(name);
  character->setUnique(attributes.value("uniq", false).toBool());
  character->setCharacterSheet(attributes["sheet"].toString());
  character->setScript(attributes.value("script", "character").toString());
  addCharacter(character);
}

void CharacterParty::addCharacter(Character* character)
{
  character->setParent(this);
  list.push_back(character);
  emit partyChanged();
}

void CharacterParty::removeCharacter(Character* character)
{
  list.removeAll(character);
  emit partyChanged();
}

bool CharacterParty::containsCharacter(Character* character)
{
  return list.count(character) > 0;
}

void CharacterParty::removeCharacter(const QString& name)
{
  for (auto it = list.begin() ; it != list.end() ;)
  {
    if ((*it)->getStatistics()->getName() == name)
      it = list.erase(it);
    else
      ++it;
  }
  emit partyChanged();
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

void CharacterParty::grantXp(unsigned int value)
{
  for (auto* character : qAsConst(list))
    character->getStatistics()->addExperience(static_cast<int>(value));
}

bool CharacterParty::insertIntoZone(LevelTask* level, TileZone* zone)
{
  auto* grid = level->getGrid();
  int characterIt = 0;

  for (auto position : zone->getPositions())
  {
    if (!grid->isOccupied(position.x(), position.y()))
    {
      Character* character = list.at(characterIt);

      level->appendObject(character);
      level->setCharacterPosition(character, position.x(), position.y());
      if (++characterIt >= list.length())
        break ;
    }
  }
  if (characterIt > 0)
    emit level->cameraFocusRequired(list.at(0));
  return characterIt == list.length();
}

bool CharacterParty::insertIntoZone(LevelTask* level, const QString &zoneName)
{
  auto* tileMap = level->getTileMap();

  for (auto* zone : tileMap->getZones())
  {
    if (zone->getName() == zoneName)
      return insertIntoZone(level, zone);
  }
  return false;
}

void CharacterParty::extractFromLevel(LevelTask* level)
{
  auto* grid = level->getGrid();

  for (Character* character : list)
  {
    character->clearCurrentZones();
    character->getFieldOfView()->reset();
    grid->removeObject(character);
    level->detachObject(character);
  }
}

void CharacterParty::save(QJsonObject& data)
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
    list << character;
  }
  if (list.length() == 0)
    qDebug() << "Fatal error: saved data did not contain any player character";
}

void CharacterParty::loadIntoLevel(LevelTask* level)
{
  auto* grid = level->getGrid();

  for (Character* character : list)
  {
    grid->moveObject(character, character->getPosition().x(), character->getPosition().y());
    level->appendObject(character);
  }
}
