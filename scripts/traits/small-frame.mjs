export var name = "small-frame";

export function onToggled(characterSheet, toggled) {
  console.log(name, "toggled");
  if (toggled)
    characterSheet.traits.push(name);
  else
    characterSheet.traits.splice(characterSheet.traits.indexOf(name), 1);
}
