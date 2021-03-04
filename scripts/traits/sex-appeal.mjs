export var name = "sex-appeal";

// should give dialogue choices with characters sexually attracted to the player's gender

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.traits.push(name);
  else
    characterSheet.traits.splice(characterSheet.traits.indexOf(name), 1);
}

