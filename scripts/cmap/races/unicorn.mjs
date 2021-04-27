import * as Pony from "./earth-pony.mjs";

export const isPlayable  = Pony.isPlayable;

export function spriteSheet(model) {
  const pony = Pony.spriteSheet(model);

  pony.overlay = "unicorn";
  return pony;
}
