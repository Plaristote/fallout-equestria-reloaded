export function isAvailableFor(characterSheet) {
  if (characterSheet.level >= 3 && characterSheet.perks.indexOf("earlierSequence") < 0)
    return characterSheet.perception >= 6;
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.sequence += 2;
  else
    characterSheet.sequence -= 2;
}
