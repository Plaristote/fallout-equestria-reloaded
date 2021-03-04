export var name = "jinxed";

// Should give a 50% chance for all misses to turn into critical failures, for everyone

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.traits.push(name);
  else
    characterSheet.traits.splice(characterSheet.traits.indexOf(name), 1);
}
