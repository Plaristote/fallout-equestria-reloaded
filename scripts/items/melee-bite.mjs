import {MeleeAttack} from "./melee.mjs";

class Bite extends MeleeAttack {
  constructor(model) {
    super(model);
    this.hitSound = "critters/bite";
  }

  getActionPointCost() {
    return 4;
  }

  getDamageRange() {
    const baseDamage = Math.ceil(this.getDamageBase()) + 2;
    const range      = Math.ceil(baseDamage / 10);

    return [baseDamage, range]; 
  }
};

export function create(model) {
  return new Bite(model);
}
