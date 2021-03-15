export var name = "fast-metabolism";

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "healingRate")
    return value + 2;
  if (name == "poisonResistance" || name == "radiationResistance")
    return 0;
  return value;
}

export function onToggled(characterSheet, toggled) {
}

