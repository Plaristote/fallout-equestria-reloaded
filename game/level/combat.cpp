#include "combat.h"
#include "game.h"
#include "game/characters/actionqueue.h"

CombatComponent::CombatComponent(QObject *parent) : VisualEffectsComponent(parent)
{
  connect(this, &InteractionComponent::activeItemChanged,   this, &CombatComponent::onActiveItemChanged);
  connect(this, &CombatComponent::currentCombattantChanged, this, &CombatComponent::updateWaitingMode);
  connect(this, &CombatComponent::combatChanged,            this, &CombatComponent::updateWaitingMode);
}

void CombatComponent::updateWaitingMode()
{
  enableWaitingMode(combat && !isCharacterTurn(getPlayer()));
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
  return combatIterator >= 0 && combattants.size() > combatIterator && combattants[combatIterator] == character;
}

void CombatComponent::onActiveItemChanged()
{
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
      startCombat(character);
    else
      sortCombattants();
    emit combattantsChanged();
    for (auto* playerPartyMember : playerParty->getCharacters())
      joinCombat(playerPartyMember);
  }
}

void CombatComponent::startCombat(Character* character)
{
  combat = true;
  emit combatChanged();
  emit currentCombattantChanged();
  initializeCharacterTurn(character);
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
  if (isPlayerTurn())
  {
    if (mouseMode != MovementCursor)
      swapMouseMode();
    if (tryToEndCombat())
      return ;
  }
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
  if (!character->isUnconscious())
  {
    character->getFieldOfView()->runTask();
    character->scriptCall("onTurnStart");
  }
  else
    onNextCombatTurn();
}

void CombatComponent::finalizeCharacterTurn(Character* character)
{
  character->getActionQueue()->reset();
  character->resetActionPoints();
  character->updateTasks(WORLDTIME_TURN_DURATION);
}

void CombatComponent::finalizeRound()
{
  Game::get()->getTimeManager()->addElapsedMilliseconds(WORLDTIME_TURN_DURATION);
  sortCombattants();
}

void CombatComponent::passTurn(Character *character)
{
  if (isCharacterTurn(character))
    onNextCombatTurn();
}
