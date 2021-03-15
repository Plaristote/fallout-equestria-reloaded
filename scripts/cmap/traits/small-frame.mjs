export var name = "small-frame";

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "carryWeight")
    return 25 + characterSheet.strength * 15;
  return value;
}

export function onToggled(characterSheet, toggled) {
  const value = toggled ? 1 : -1;

  characterSheet.agility += value;
}
