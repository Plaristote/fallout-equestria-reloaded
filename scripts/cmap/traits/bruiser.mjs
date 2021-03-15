export var name = "bruiser";

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "actionPoints")
    return value - 2;
  return value;
}

export function onToggled(characterSheet, toggled) {
  const value = toggled ? 2 : -2;

  characterSheet.strength += value;
}
