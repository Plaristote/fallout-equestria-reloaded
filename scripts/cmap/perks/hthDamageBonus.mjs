export function isAvailableFor(characterSheet) {
  var count = 0;
  for (var i = 0 ; i < characterSheet.perks.length ; ++i) {
    if (characterSheet.perks[i] === "hthDamageBonus") { count++; }
  }

  if (characterSheet.level >= 3 && count < 3)
    return characterSheet.strength >= 6 && characterSheet.agility >= 6;
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.meleeDamage += 2;
  else
    characterSheet.meleeDamage -= 2;
}
