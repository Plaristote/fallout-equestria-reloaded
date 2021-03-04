export var name = "kamikaze";

export function modifyBaseStatistic(characterSheet, name, value) {
  if (name == "sequence")
    return value + 5;
  if (name == "armorClass")
    return 0;
  return value;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.traits.push(name);
  else
    characterSheet.traits.splice(characterSheet.traits.indexOf(name), 1);
}

