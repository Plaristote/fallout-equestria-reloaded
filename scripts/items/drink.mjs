import {Consumable} from "./consumable.mjs";

export class Drink extends Consumable {
  constructor(model) {
    super(model);
  }
}

export function create(model) {
  return new Drink(model);
}
