#include "combat.h"
#include "game.h"
#define WORLDTIME_TURN_DURATION_IN_SECONDS 10
#define WORLDTIME_TURN_DURATION WORLDTIME_TURN_DURATION_IN_SECONDS * 1000

CombatComponent::CombatComponent(QObject *parent) : TextBubblesComponent(parent)
{

}

void CombatComponent::joinCombat(Character* character)
{
  if (!isInCombat(character))
  {
    character->resetActionPoints();
    combattants << character;
    if (combat == false)
    {
      combat = true;
      emit combatChanged();
    }
    else
      sortCombattants();
    emit combattantsChanged();
  }
}

void CombatComponent::leaveCombat(Character* character)
{
  auto index = combattants.indexOf(character);

  if (index == combatIterator)
    onNextCombatTurn();
  if (index <= combatIterator)
    combatIterator--;
  combattants.removeAll(character);
  emit combattantsChanged();
}

bool CombatComponent::tryToEndCombat()
{
  if (isCombatEnabled()) { return false; }
  combat = false;
  return true;
}

void CombatComponent::sortCombattants()
{
  std::sort(combattants.begin(), combattants.end(), [](Character* a, Character* b) -> bool
  {
    return a->getStatistics()->get_sequence() < b->getStatistics()->get_sequence();
  });
}

void CombatComponent::removeDeadCombattants()
{
  for (auto it = combattants.begin() ; it != combattants.end() ;)
  {
    Character* character = *it;

    if (!character->isAlive())
      it = combattants.erase(it);
    else
      ++it;
  }
}

void CombatComponent::onNextCombatTurn()
{
  Character* previous;
  Character* current;

  previous = combattants.at(combatIterator);
  combatIterator = combatIterator + 1 >= combattants.size() ? 0 : combatIterator  + 1;
  current  = combattants.at(combatIterator);
  if (previous && previous->isAlive())
    finalizeCharacterTurn(previous);
  if (combatIterator == 0)
    finalizeRound();
  if (!tryToEndCombat())
    initializeCharacterTurn(current);
}

void CombatComponent::initializeCharacterTurn(Character* character)
{

}

void CombatComponent::finalizeCharacterTurn(Character* character)
{
  character->resetActionPoints();
  character->getTaskManager()->update(WORLDTIME_TURN_DURATION);
}

void CombatComponent::finalizeRound()
{
  Game::get()->getTimeManager()->addElapsedMilliseconds(WORLDTIME_TURN_DURATION);
}

void CombatComponent::onMovementFinished(Character* character)
{
  if (combat)
  {
    character->useActionPoints(1, "movement");
    if (character->getActionPoints() == 0)
      character->rcurrentPath().clear();
  }
  GridComponent::onMovementFinished(character);
}

void CombatComponent::onCombattantReachedDestination()
{

}
