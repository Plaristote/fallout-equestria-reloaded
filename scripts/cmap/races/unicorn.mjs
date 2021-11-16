import * as Pony from "./earth-pony.mjs";

export const faces = ["mare-basic", "stallion-basic"];

export const isPlayable  = Pony.isPlayable;

export const withFaceColor = Pony.withFaceColor;

export function spriteSheet(model) {
  const pony = Pony.spriteSheet(model);

  pony.overlay = "unicorn";
  return pony;
}

export const itemSlots = Pony.itemSlots;

export const getDefaultItem = Pony.getDefaultItem;