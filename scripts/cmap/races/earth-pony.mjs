import {getSpriteColor} from "../helpers/spriteColor.mjs";

export const isPlayable = true;

export function spriteSheet(model) {
  return {
    cloneOf: "pony",
    //base:    "earthpony-" + getSpriteColor(model.faceColor)
    base:    "earthpony-gray",
    color:   model.faceColor 
  };
}

export function onToggled(statistics, toggled) {
  var modifier = toggled ? 1 : -1;

  statistics.endurance += 1 * modifier;
  statistics.strength  += 1 * modifier;
}
