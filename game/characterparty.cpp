#include "characterparty.h"
#include "leveltask.h"

CharacterParty::CharacterParty(QObject *parent) : QObject(parent)
{

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

bool CharacterParty::insertIntoZone(LevelTask* level, TileZone* zone)
{
  auto* grid = level->getGrid();
  int characterIt = 0;

  for (auto position : zone->getPositions())
  {
    if (!grid->isOccupied(position.x(), position.y()))
    {
      Character* character = list.at(characterIt);

      level->registerDynamicObject(character);
      level->forceCharacterPosition(character, position.x(), position.y());
      if (++characterIt >= list.length())
        break ;
    }
  }
  return characterIt == list.length();
}

bool CharacterParty::insertIntoZone(LevelTask* level, const QString &zoneName)
{
  auto* tileMap = level->getTileMap();

  for (auto* zone : tileMap->getZones())
  {
    qDebug() << "Detected" << zone->getType() << "zone" << zone->getName() << zone->getIsDefault();
    if (zone->getName() == zoneName)
      return insertIntoZone(level, zone);
  }
  return false;
}

void CharacterParty::extractFromLevel(LevelTask* level)
{
  auto* grid = level->getGrid();

  for (Character* character : list)
    grid->removeObject(character);
}
