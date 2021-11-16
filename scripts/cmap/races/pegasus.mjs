import * as Pony from "./earth-pony.mjs";

export const isPlayable  = Pony.isPlayable;

export const withFaceColor = Pony.withFaceColor;

export const faces = ["mare-basic", "stallion-basic"];

export function spriteSheet(model) {
  const pony = Pony.spriteSheet(model);

  pony.overlay = "pegasus";
  return pony;
}

export const itemSlots = Pony.itemSlots;

export const getDefaultItem = Pony.getDefaultItem;

export function onToggled(statistics, toggled) {
  var modifier = toggled ? 1 : -1;

  statistics.agility += 1 * modifier;
}

export function modifyBaseStatistic(statistics, name, value) {
  if (name == "carryWeight")
    return value - 25;
  if (name == "armorClass")
    return value + 5;
  if (name === "sequence")
    return value + 5;
  return value;
}
