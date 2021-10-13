export function isAvailableFor(characterSheet) {
  return characterSheet.level >= 3 && characterSheet.perks.indexOf("thief") < 0;
}

export function onToggled(characterSheet, toggled) {
  var bonus = 10;

  if (!toggled)
    bonus = -bonus;
  characterSheet.sneak    += bonus;
  characterSheet.steal    += bonus;
  characterSheet.lockpick += bonus;
  characterSheet.traps    += bonus;
}
