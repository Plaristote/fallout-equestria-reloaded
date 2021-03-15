export var name = "gifted";

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "skillRate")
    return value - 5;
  return value;
}

export function modifyBaseSkill(characterSheet, name, value) {
  return value - 10;
}

export function onToggled(characterSheet, toggled) {
  const value = toggled ? 1 : -1;

  characterSheet.strength += value;
  characterSheet.perception += value;
  characterSheet.endurance += value;
  characterSheet.charisma += value;
  characterSheet.intelligence += value;
  characterSheet.agility += value;
  characterSheet.luck += value;
}
