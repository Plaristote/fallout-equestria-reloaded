import {Medikit} from "./medikit.mjs";

class HealthPotion extends Medikit {
  constructor(model) {
    super(model);
    this.minHeal = 12;
    this.maxHeal = 27;
  }
}

export function create(model) {
  return new HealthPotion(model);
}
