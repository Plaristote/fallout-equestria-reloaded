export var name = "chem-resistant";

// Should have the effects of drugs and addiction chances

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.traits.push(name);
  else
    characterSheet.traits.splice(characterSheet.traits.indexOf(name), 1);
}
