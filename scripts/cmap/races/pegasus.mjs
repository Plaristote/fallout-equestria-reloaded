import {getSpriteColor} from "../helpers/spriteColor.mjs";

export const isPlayable = true;

export function spriteSheet(model) {
  return "pegasus-" + getSpriteColor(model.faceColor);
}

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
