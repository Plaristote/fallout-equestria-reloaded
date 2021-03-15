export var name = "skilled";

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "perkRate")
    return value + 1;
  if (name == "skillRate")
    return value + 5;
  return value;
}

export function onToggled(characterSheet, toggled) {
  console.log(name, "toggled");
}
