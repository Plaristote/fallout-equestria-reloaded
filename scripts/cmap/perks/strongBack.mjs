export function isAvailableFor(characterSheet) {
  if (characterSheet.level >= 3)
    return characterSheet.strength >= 6 && characterSheet.endurance >= 6;
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.carryWeight += 50;
  else
    characterSheet.carryWeight -= 50;
}
