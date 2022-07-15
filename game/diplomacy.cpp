#include "diplomacy.hpp"
#include <algorithm>
#include <QJsonArray>

using namespace std;

WorldDiplomacy::WorldDiplomacy(DataEngine& de) : _data_engine(de)
{
  _next_flag = 1;
}

void WorldDiplomacy::initialize(void)
{
  QJsonObject factions = _data_engine.getWorldDiplomacy();

  _factions.clear();
  for (auto it = factions.begin() ; it != factions.end() ; ++it)
    addFaction(it.key());
  for (auto faction = factions.begin() ; faction != factions.end() ; ++faction)
  {
    auto enemies = faction->toObject()["enemies"].toObject();

    for (auto enemy = enemies.begin() ; enemy != enemies.end() ; ++enemy)
      setAsEnemy(enemy.value().toBool(), enemy.key(), faction.key());
  }
}

void WorldDiplomacy::addFaction(const QString& name)
{
  Faction faction;

  faction.flag      = _next_flag;
  faction.name      = name;
  faction.enemyMask = 0;
  _next_flag       *= 2;
  _factions.push_back(faction);
  _data_engine.registerFaction(name);
}

WorldDiplomacy::Faction* WorldDiplomacy::getFaction(const QString& name)
{
  auto it = find(_factions.begin(), _factions.end(), name);

  if (it != _factions.end())
    return &(*it);
  return nullptr;
}

WorldDiplomacy::Faction* WorldDiplomacy::getFaction(unsigned int flag)
{
  auto it = find(_factions.begin(), _factions.end(), flag);

  if (it != _factions.end())
    return &(*it);
  return nullptr;
}

bool WorldDiplomacy::areEnemies(const QString& name1, const QString& name2) const
{
  Factions::const_iterator it_first  = find(_factions.begin(), _factions.end(), name1);
  Factions::const_iterator it_second = find(_factions.begin(), _factions.end(), name2);

  if (it_first != _factions.end() && it_second != _factions.end())
    return (it_first->flag & it_second->enemyMask) > 0 ||
           (it_second->flag & it_first->enemyMask) > 0;
  return false;
}

void WorldDiplomacy::setAsEnemy(bool set, const QString& name1, const QString& name2)
{
  Factions::iterator it_first  = find(_factions.begin(), _factions.end(), name1);
  Factions::iterator it_second = find(_factions.begin(), _factions.end(), name2);

  if (it_first != _factions.end() && it_second != _factions.end())
    setAsEnemy(set, *it_first, *it_second);
}

void WorldDiplomacy::setAsEnemy(bool set, unsigned int flag_1, unsigned int flag_2)
{
  auto it_first  = find(_factions.begin(), _factions.end(), flag_1);
  auto it_second = find(_factions.begin(), _factions.end(), flag_2);

  if (it_first != _factions.end() && it_second != _factions.end())
    setAsEnemy(set, *it_first, *it_second);
}

void WorldDiplomacy::setAsEnemy(bool set, Faction& first, Faction& second)
{
  QJsonObject factions = _data_engine.getWorldDiplomacy();
  QJsonObject firstData = factions[first.name].toObject();
  QJsonObject secondData = factions[second.name].toObject();

  if (set)
  {
    first.enemyMask  |= second.flag;
    second.enemyMask |= first.flag;
  }
  else
  {
    if (first.enemyMask & second.flag)
      first.enemyMask -= second.flag;
    if (second.enemyMask & first.flag)
      second.enemyMask -= first.flag;
  }
  firstData["enemies"].toObject().insert(second.name, set);
  secondData["enemies"].toObject().insert(first.name, set);
  _data_engine.setWorldDiplomacy(factions);
  emit update({first.name, second.name}, set);
}
