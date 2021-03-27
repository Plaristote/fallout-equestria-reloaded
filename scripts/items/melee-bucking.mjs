import {MeleeAttack} from "./melee.mjs";

class Bucking extends MeleeAttack {
  getActionPointCost() {
    return 4;
  }

  getDamageRange() {
    return [Math.ceil(this.getDamageBase() * 1.5), 5]; 
  }
};

export function create(model) {
  return new Bucking(model);
}
