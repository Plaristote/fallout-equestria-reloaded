#include "combat.h"
#include "game.h"
#include "game/characters/actionqueue.h"

double CombatComponent::combatSpeedOption = 1.0;

CombatComponent::CombatComponent(QObject *parent) : VisualEffectsComponent(parent)
{
  connect(this, &InteractionComponent::activeItemChanged,   this, &CombatComponent::onActiveItemChanged);
  connect(this, &CombatComponent::currentCombattantChanged, this, &CombatComponent::updateWaitingMode);
  connect(this, &CombatComponent::combatChanged,            this, &CombatComponent::updateWaitingMode);
  connect(this, &CombatComponent::combatChanged,            this, &CombatComponent::onCombatStateChanged);
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

bool CombatComponent::isCombatEnabled() const
{
  if (combattants.size() > 1 && combat == true)
  {
    auto check = [](const Character* character) { return character->shouldJoinFight(); };
    auto it    = std::find_if(combattants.begin(), combattants.end(), check);

    return it != combattants.end();
  }
  return false;
}

bool CombatComponent::tryToEndCombat()
{  
  if (!isCombatEnabled())
  {
    combat = false;
    combatIterator = 0;
    combattants.clear();
    emit combatChanged();
    emit combattantsChanged();
    return true;
  }
  return false;
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
  }
  qDebug() << "CombatComponent::onNextCombatTurn" << combatIterator;
  combatIterator = combatIterator + 1 >= combattants.size() ? 0 : combatIterator  + 1;
  current  = combattants.at(combatIterator);
  if (previous && previous->isAlive())
    finalizeCharacterTurn(previous);
  if (combatIterator == 0)
  {
    qDebug() << "CombatComponent::finalizeRound";
    finalizeRound();
    if (!tryToEndCombat())
      initializeCharacterTurn(current);
  }
  else
    initializeCharacterTurn(current);
  emit currentCombattantChanged();
}

void CombatComponent::onCharacterDied(Character* character)
{
  int characterIt = combattants.indexOf(character);

  finalizeArmorClassBonus(character);
  if (characterIt >= 0)
  {
    if (combattants.size() == 1)
      tryToEndCombat();
    else if (isCharacterTurn(character))
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

void CombatComponent::initializeArmorClassBonus(Character* character)
{
  int armorClass = character->getStatistics()->get_armorClass();

  armorClassBonuses.insert(character, character->getActionPoints());
  character->getStatistics()->set_armorClass(armorClass + character->getActionPoints());
}

void CombatComponent::finalizeArmorClassBonus(Character* character)
{
  auto it = armorClassBonuses.find(character);

  if (it != armorClassBonuses.end())
  {
    int armorClass = character->getStatistics()->get_armorClass();

    character->getStatistics()->set_armorClass(armorClass - it.value());
    armorClassBonuses.erase(it);
  }
  armorClassBonuses.remove(character);
}

void CombatComponent::finalizeAllArmorClassBonus()
{
  for (Character* character : armorClassBonuses.keys())
    finalizeArmorClassBonus(character);
}

void CombatComponent::initializeCharacterTurn(Character* character)
{
  if (!character->isUnconscious())
  {
    if (character == getPlayer())
      Game::get()->getLevel()->getSoundManager()->play("start-turn");
    character->getFieldOfView()->runTask();
    finalizeArmorClassBonus(character);
    character->scriptCall("onTurnStart");
  }
  else
    onNextCombatTurn();
}

void CombatComponent::finalizeCharacterTurn(Character* character)
{
  if (character == getPlayer())
    Game::get()->getLevel()->getSoundManager()->play("end-turn");
  initializeArmorClassBonus(character);
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

void CombatComponent::onCombatStateChanged()
{
  Game::get()->getLevel()->getSoundManager()->play(combat ? "start-combat" : "end-combat");
  getPlayer()->getActionQueue()->reset();
  if (!combat)
    finalizeAllArmorClassBonus();
  if (script)
  {
    if (combat && script->hasMethod("onCombatStarted"))
      script->call("onCombatStarted");
    else if (!combat && script->hasMethod("onCombatEnded"))
      script->call("onCombatEnded");
  }
}
