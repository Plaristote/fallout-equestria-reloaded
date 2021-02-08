export var name = "gifted";

export function onToggled(characterSheet, toggled) {
  const value = toggled ? 1 : -1;

  characterSheet.strength += value;
  characterSheet.perception += value;
  characterSheet.endurance += value;
  characterSheet.charisma += value;
  characterSheet.intelligence += value;
  characterSheet.agility += value;
  characterSheet.luck += value;
  if (toggled)
    characterSheet.traits.push(name);
  else
    characterSheet.traits.splice(characterSheet.traits.indexOf(name), 1);
}
