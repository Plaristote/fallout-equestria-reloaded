export const spriteSheet = "wolf";

export function onToggled(statistics, toggled) {
  let modifier = toggled ? 1 : -1;

  statistics.strength     += modifier;
  statistics.perception   += (modifier * 2);
  statistics.endurance    -= (modifier * 4);
  statistics.charisma     -= (modifier * 3);
  statistics.intelligence -= modifier;
  statistics.agility      += (modifier * 3);
}

export function modifyBaseSkill(characterSheet, name, value) {
  if (name == "unarmed")
    return value + 30;
  return value;
}
