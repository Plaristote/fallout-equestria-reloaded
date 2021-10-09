import {MeleeAttack} from "./melee.mjs";
import {randomCheck} from "../behaviour/random.mjs";

class Stinger extends MeleeAttack {
  constructor(model) {
    super(model);
    this.hitSound = "critters/bite";
  }

  getDamageType() {
    return "piercing";
  }

  getDamageRange() {
    const baseDamage = Math.ceil(this.getDamageBase()) + 1;
    const range      = Math.ceil(baseDamage / 10);

    return [baseDamage, range]; 
  }

  useOn(target) {
    if (super.useOn(target)) {
      randomCheck(target.statistics.poisonResistance, {
        failure:         this.applyPoison.bind(this, target),
        criticalFailure: this.applyPoison.bind(this, target)
      });
      return true;
    }
    return false;
  }

  applyPoison(target) {
    const currentLevel = target.hasVariable("poisonLevel") ? target.getVariable("poisonLevel") : 0;

    target.setVariable("poisonLevel", Math.max(currentLevel, this.user.statistics.level));
    console.log("Zet poizon level", currentLevel, this.user.statistics.level);
    target.addBuff("poisoned");
  }
};

export function create(model) {
  return new Stinger(model);
}

