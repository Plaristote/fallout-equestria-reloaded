#include "combat.h"
#include "game.h"
#include "game/characters/actionqueue.h"
#define WORLDTIME_TURN_DURATION_IN_SECONDS 10
#define WORLDTIME_TURN_DURATION WORLDTIME_TURN_DURATION_IN_SECONDS * 1000

CombatComponent::CombatComponent(QObject *parent) : TextBubblesComponent(parent)
{
  connect(this, &InteractionComponent::activeItemChanged, this, &CombatComponent::onActiveItemChanged);
}

void CombatComponent::registerDynamicObject(DynamicObject* object)
{
  TextBubblesComponent::registerDynamicObject(object);
  if (object->isCharacter())
  {
    auto* character = dynamic_cast<Character*>(object);

    addCharacterObserver(
      character,
      connect(character, &Character::requireJoinCombat, [this, character]() { joinCombat(character); })
    );
  }
}

void CombatComponent::unregisterDynamicObject(DynamicObject* object)
{
  TextBubblesComponent::unregisterDynamicObject(object);
}

bool CombatComponent::isPlayerTurn() const
{
  return isCharacterTurn(Game::get()->getPlayer());
}

bool CombatComponent::isCharacterTurn(Character *character) const
{
  return combattants[combatIterator] == character;
}

void CombatComponent::onActiveItemChanged()
{
  qDebug() << "onActiveItemChaned";
  if (activeItem && activeItem->isCombatItem())
    joinCombat(Game::get()->getPlayer());
}

void CombatComponent::joinCombat(Character* character)
{
  if (!isInCombat(character))
  {
    auto* playerParty = Game::get()->getPlayerParty();

    character->resetActionPoints();
    combattants << character;
    if (combat == false)
    {
      combat = true;
      emit combatChanged();
      emit currentCombattantChanged();
      initializeCharacterTurn(character);
    }
    else
      sortCombattants();
    emit combattantsChanged();
    for (auto* playerPartyMember : playerParty->getCharacters())
      joinCombat(playerPartyMember);
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
  combatIterator = 0;
  combattants.clear();
  emit combatChanged();
  emit combattantsChanged();
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
  if (isPlayerTurn() && tryToEndCombat())
    return ;
  combatIterator = combatIterator + 1 >= combattants.size() ? 0 : combatIterator  + 1;
  current  = combattants.at(combatIterator);
  if (previous && previous->isAlive())
    finalizeCharacterTurn(previous);
  if (combatIterator == 0)
    finalizeRound();
  if (!tryToEndCombat())
    initializeCharacterTurn(current);
  emit currentCombattantChanged();
}

void CombatComponent::onCharacterDied(Character* character)
{
  int characterIt = combattants.indexOf(character);

  if (characterIt >= 0)
  {
    if (isCharacterTurn(character))
    {
      onNextCombatTurn();
      combattants.removeAll(character);
      combatIterator--;
    }
    else if (characterIt < combatIterator)
    {
      combattants.removeAll(character);
      combatIterator--;
    }
    else
      combattants.removeAll(character);
    emit combattantsChanged();
  }
  TextBubblesComponent::onCharacterDied(character);
}

void CombatComponent::initializeCharacterTurn(Character* character)
{
  character->getFieldOfView()->runTask();
  character->scriptCall("onTurnStart");
}

void CombatComponent::finalizeCharacterTurn(Character* character)
{
  character->getActionQueue()->reset();
  character->resetActionPoints();
  character->getTaskManager()->update(WORLDTIME_TURN_DURATION);
}

void CombatComponent::finalizeRound()
{
  Game::get()->getTimeManager()->addElapsedMilliseconds(WORLDTIME_TURN_DURATION);
  sortCombattants();
}

void CombatComponent::onMovementFinished(Character* character)
{
  if (combat)
  {
    character->useActionPoints(1, "movement");
    if (character->getActionPoints() == 0)
    {
      character->rcurrentPath().clear();
      character->setAnimation("idle-down");
    }
  }
  GridComponent::onMovementFinished(character);
}

void CombatComponent::onCombattantReachedDestination()
{

}

void CombatComponent::passTurn(Character *character)
{
  if (isCharacterTurn(character))
    onNextCombatTurn();
}
