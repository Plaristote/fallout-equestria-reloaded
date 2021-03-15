export var name = "finesse";

// Should also add 30% damage resistance on targets

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "criticalChance")
    return value + 10;
  return value;
}

export function onToggled(characterSheet, toggled) {
  console.log(name, "toggled");
}

