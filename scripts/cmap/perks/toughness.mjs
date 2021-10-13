export function isAvailableFor(characterSheet) {
  if (characterSheet.level >= 3)
    return characterSheet.endurance >= 6 && characterSheet.luck >= 6;
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.damageResistance += 10;
  else
    characterSheet.damageResistance -= 10;
}
