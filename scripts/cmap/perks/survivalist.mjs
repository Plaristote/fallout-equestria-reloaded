export function isAvailableFor(characterSheet) {
  if (characterSheet.level >= 3)
    return characterSheet.endurance >= 6 && characterSheet.intelligence >= 6 && characterSheet.outdoorsman >= 40;
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.outdoorsman += 40;
  else
    characterSheet.outdoorsman -= 40;
}
