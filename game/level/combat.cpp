#include "combat.h"

CombatComponent::CombatComponent(QObject *parent) : TextBubblesComponent(parent)
{

}

void CombatComponent::joinCombat(Character* character)
{
  combattants << character;
  if (combattants.length() > 1 && combat == false)
  {
    sortCombattants();
    combat = true;
    emit combatChanged();
  }
}

void CombatComponent::leaveCombat(Character* character)
{
  combattants.removeAll(character);
  tryToEndCombat();
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

    if (character->getStatistics()->getHitPoints() > 0)
      it = combattants.erase(it);
    else
      ++it;
  }
}
