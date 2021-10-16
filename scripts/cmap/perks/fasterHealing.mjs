export function isAvailableFor(characterSheet) {
  if (characterSheet.level >= 3 && characterSheet.perks.indexOf("fasterHealing") < 0)
    return characterSheet.endurance >= 6;
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.healingRate += 2;
  else
    characterSheet.healingRate -= 2;
}
