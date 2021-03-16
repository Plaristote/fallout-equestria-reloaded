import {getSpriteColor} from "../helpers/spriteColor.mjs";

export const isPlayable = true;

export function spriteSheet(model) {
  return "unicorn-" + getSpriteColor(model.faceColor);
}
