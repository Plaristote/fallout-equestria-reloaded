export function isAvailableFor(characterSheet) {
  if (characterSheet.perks.indexOf("awareness") < 0)
    return characterSheet.perception >= 5 && characterSheet.level >= 3;
  return false;
}

export function awarenessHint(character) {
  var html = `${character.statistics.name}<br/>`
           + `${character.statistics.hitPoints} /`
	   + `${character.statistics.maxHitPoints} ${i18n.t('HP')}<br/>`;

  for (var i = 0 ; i < character.inventory.slotNames.length ; ++i) {
    const slotName = character.inventory.slotNames[i];
    const item = character.inventory.getEquippedItem(slotName);
    const label = slotName.startsWith("use") ? "" : (i18n.t(`item-slots.${slotName}`) + ": ");

    if (item)
      html += label + i18n.t(`items.${item.itemType}`) + "<br/>";
  }
  return `<i style="font-size:8px">${html}</i>`;
}
