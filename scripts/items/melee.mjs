import {Weapon} from "./weapon.mjs";

export class MeleeAttack extends Weapon {
  getActionPointCost() {
    return 3;
  }

  getDamageType() {
    return "blunt";
  }

  getDamageRange() {
    const base = this.getDamageBase();

    return [base, 3];
  }

  getDamageBase() {
    if (this.user)
      return this.user.statistics.meleeDamage;
    return 3;
  }
};

export function create() {
  return new MeleeAttack();
}
