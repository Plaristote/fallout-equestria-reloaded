export function isAvailableFor(characterSheet) {
  return false;
}

export function onToggled(characterSheet, toggled) {
  if (toggled)
    characterSheet.speech += 15;
  else
    characterSheet.speech -= 15;
}

export function increaseBookCount(character) {
  var count = character.hasVariable("books") ? character.getVariable("books") : 0;

  console.log("increaseBookCount", count);
  character.setVariable("books", ++count);
  if (count > 30 && character.statistics.perks.indexOf("bookworm") < 0) {
    character.statistics.togglePerk("bookworm", true);
    if (character === game.player)
      game.appendToConsole(i18n.t("messages.gained-bookworm"));
  }
}
