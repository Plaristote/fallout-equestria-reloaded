export var name = "heavy-hoofed";

// Melee critical hits should also deal 30% less damage

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "meleeDamage")
    return value + 4;
  return value;
}

export function onToggled(characterSheet, toggled) {
  console.log(name, "toggled");
}
