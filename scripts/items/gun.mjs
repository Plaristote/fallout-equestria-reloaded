import {Weapon} from "./weapon.mjs";

export class Handgun extends Weapon {
  constructor(model) {
    super(model);
    this.skill = "smallGuns";
  }

  getActionPointCost() {
    return 5;
  }

  getDamageRange() {
    return [8,11];
  }

  getRange() {
    return 4;
  }
};

export function create(model) {
  return new Handgun(model);
}
