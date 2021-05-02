import {Weapon} from "./weapon.mjs";

export class MeleeAttack extends Weapon {
  constructor(model) {
    super(model);
    this.useModes = ["hit"];
  }

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

  getAnimationSteps(target) {
    return [{ type: "Animation", animation: "melee", object: this.user }];
  }

  getUseAnimation() {
    return [{ type: "Animation", animation: "melee", object: this.user }];
  }
};

export function create(model) {
  return new MeleeAttack(model);
}
