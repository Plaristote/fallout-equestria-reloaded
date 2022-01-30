#include "field_of_view.hpp"
#include "../character.h"
#include "game/dices.hpp"
#include "game.h"
#include <algorithm>

using namespace std;

FieldOfView::FieldOfView(Character& character) : QObject(&character), character(character)
{
  interval = 1;
  timeLeft = 0;
}

FieldOfView::~FieldOfView()
{
}

QJSValue FieldOfView::getEnemies() const
{
  auto& engine = Game::get()->getScriptEngine();
  QJSValue retval = engine.newArray();
  QJSValue push = retval.property("push");

  for (Character* character : GetDetectedEnemies())
    push.callWithInstance(retval, QJSValueList() << character->asJSValue());
  return retval;
}

QJSValue FieldOfView::getCharactersInRange() const
{
  auto& engine = Game::get()->getScriptEngine();
  QJSValue retval = engine.newArray();
  QJSValue push = retval.property("push");

  for (Character* character : GetCharactersInRange())
    push.callWithInstance(retval, QJSValueList() << character->asJSValue());
  return retval;
}

QJSValue FieldOfView::getAllies() const
{
  auto& engine = Game::get()->getScriptEngine();
  QJSValue retval = engine.newArray();
  QJSValue push = retval.property("push");

  for (Character* character : GetDetectedAllies())
    push.callWithInstance(retval, QJSValueList() << character->asJSValue());
  return retval;
}

QJSValue FieldOfView::getNonHostiles() const
{
  auto& engine = Game::get()->getScriptEngine();
  QJSValue retval = engine.newArray();
  QJSValue push = retval.property("push");

  for (Character* character : GetDetectedNonHostile())
    push.callWithInstance(retval, QJSValueList() << character->asJSValue());
  return retval;
}

QJSValue FieldOfView::getCharacters() const
{
  auto& engine = Game::get()->getScriptEngine();
  QJSValue retval = engine.newArray();
  QJSValue push = retval.property("push");

  for (Character* character : GetDetectedCharacters())
    push.callWithInstance(retval, QJSValueList() << character->asJSValue());
  return retval;
}

void FieldOfView::SetIntervalDurationFromStatistics(void)
{
  unsigned short   duration   = 3;
  const StatModel* statistics = character.getStatistics();

  if (statistics)
  {   
    unsigned short perception = static_cast<unsigned short>(statistics->property("perception").toInt());

    duration = (10 - perception) / 2;
  }
  interval = (duration != 0 ? duration : 1) * 1000;
  timeLeft = interval;
}

void FieldOfView::update(qint64 delta)
{
  if (timeLeft <= delta && character.getStatistics()->getHitPoints() > 0)
    runTask();
  else
  {
    timeLeft -= delta;
    emit updated();
  }
}

void FieldOfView::runTask()
{
  SetIntervalDurationFromStatistics();
  LoseTrackOfCharacters(detected_enemies);
  LoseTrackOfCharacters(detected_characters);
  detectCharacters();
  timeLeft = interval;
  emit refreshed();
}

void FieldOfView::reset()
{
  detected_characters.clear();
  detected_enemies.clear();
}

void FieldOfView::removeCharacter(Character* character)
{
  for (auto it = detected_characters.begin() ; it != detected_characters.end() ;)
  {
    if (it->character == character)
    {
      it = detected_characters.erase(it);
      break ;
    }
    ++it;
  }
  for (auto it = detected_enemies.begin() ; it != detected_enemies.end() ;)
  {
    if (it->character == character)
    {
      it = detected_enemies.erase(it);
      break ;
    }
    ++it;
  }
}

FieldOfView::CharacterList FieldOfView::GetDetectedCharacters(void) const
{
  CharacterList list;

  AppendEntriesToCharacterList(detected_enemies,    list);
  AppendEntriesToCharacterList(detected_characters, list);
  return list;
}

FieldOfView::CharacterList FieldOfView::GetDetectedEnemies(void) const
{
  CharacterList list;

  AppendEntriesToCharacterList(detected_enemies, list);
  return list;
}

FieldOfView::CharacterList FieldOfView::GetDetectedCharactersMatching(std::function<bool (Character*)> functor) const
{
  CharacterList list = GetDetectedCharacters();
  
  for (auto it = list.begin() ; it != list.end() ;)
  {
    if (functor(*it))
      ++it;
    else
      it = list.erase(it);
  }
  return list;
}

FieldOfView::CharacterList FieldOfView::GetDetectedNonHostile(void) const
{
  return (GetDetectedCharactersMatching([this](Character* detected_character) -> bool
  {
    return !(character.isAlly(detected_character)) && !(character.isEnemy(detected_character));
  }));
}

FieldOfView::CharacterList FieldOfView::GetDetectedAllies(void) const
{
  return GetDetectedCharactersMatching([this](Character* detected_character) -> bool
  {
    return character.isAlly(detected_character);
  });
}

void FieldOfView::AppendEntriesToCharacterList(const std::list<Entry>& entries, CharacterList& list) const
{
  for (auto it = entries.begin() ; it != entries.end() ; ++it)
  {
    if (it->character)
      list.push_back(it->character);
  }
}

bool FieldOfView::hasLivingEnemiesInSight(void) const
{
  for (auto iterator = detected_enemies.begin() ; iterator != detected_enemies.end() ; ++iterator)
  {
    if ((**iterator).isAlive())
      return true;
  }
  return false;
}

bool FieldOfView::isDetected(const Character* character_to_check) const
{
  if (character.isEnemy(character_to_check))
    return IsCharacterInList(character_to_check, detected_enemies);
  return IsCharacterInList(character_to_check, detected_characters);
}

bool FieldOfView::IsCharacterInList(const Character* character_to_check, const std::list<Entry>& list) const
{
  return (find(list.begin(), list.end(), character_to_check) != list.end());
}

void FieldOfView::detectCharacters()
{
  CharacterList characters_in_range = GetCharactersInRange();
  auto          iterator            = characters_in_range.begin();

  for (; iterator < characters_in_range.end() ; ++iterator)
  {
    Character* checking_character = *iterator;

    if (checking_character != &character)
    {
      if (character.isAlly(checking_character) || !checking_character->isAlive())
        setCharacterDetected(checking_character);
      else if (character.hasLineOfSight(checking_character) && CheckIfEnemyIsDetected(*checking_character))
      {
        if (character.isEnemy(checking_character) && checking_character->isAlive())
          setEnemyDetected(checking_character);
        else
          setCharacterDetected(checking_character);
      }
    }
  }
}

bool FieldOfView::CheckIfEnemyIsDetected(Character& enemy) const
{
  return !(enemy.isSneaking()) || CheckIfSneakingEnemyIsDetected(enemy);
}

bool FieldOfView::CheckIfSneakingEnemyIsDetected(Character& enemy) const
{
  return enemy.tryDetection(&character);
}

void FieldOfView::setEnemyDetected(Character* enemy)
{
  InsertOrUpdateCharacterInList(*enemy, detected_enemies);
}

void FieldOfView::setCharacterDetected(Character* character)
{
  InsertOrUpdateCharacterInList(*character, detected_characters);
}

void FieldOfView::InsertOrUpdateCharacterInList(Character& character, std::list<Entry>& list)
{
  auto iterator = find(list.begin(), list.end(), &character);
  
  if (iterator != list.end())
    iterator->time_to_live = FOV_TTL;
  else
  {
    list.push_back(&character);
    emit characterDetected(&character);
  }
}

float FieldOfView::GetRadius() const
{
  const StatModel* stat_controller = character.getStatistics();
  int              perception      = 5;

  if (stat_controller)
    perception = stat_controller->property("perception").toInt();
  return static_cast<float>(2 + perception * 3);
}

FieldOfView::CharacterList FieldOfView::GetCharactersInRange() const
{
  float field_of_view_radius = GetRadius();
  auto* level = Game::get()->getLevel();

  return level->findCharacters([this, field_of_view_radius](Character& other) -> bool
  {
    return (&other != &character && other.getDistance(&character) < field_of_view_radius);
  });
}

void FieldOfView::LoseTrackOfCharacters(std::list<Entry>& entries)
{
  auto character = entries.begin();

  while (character != entries.end())
  {
    character->time_to_live--;
    if (character->time_to_live < 0 || !(character->character->isAlive()))
      character = entries.erase(character);
    else
      character++;
  }
}
 
