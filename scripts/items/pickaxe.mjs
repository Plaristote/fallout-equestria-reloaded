import {MeleeAttack} from "./melee.mjs";

export class Pickaxe extends MeleeAttack {
  constructor(model) {
    super(model);
    this.useModes.push("mine");
    this.skill = "meleeWeapons";
    this.defaultUseMode = "mine";
  }

  get triggersCombat() {
    return this.model.useMode === "hit";
  }

  getAnimationSteps() {
    if (this.model.useMode === "hit")
      return this.getUseAnimation();
    return [{ type: "Animation", animation: "use", object: this.user }];
  }

  isValidTarget(object) {
    if (this.model.useMode === "mine")
      return object.objectName.startsWith("gemOre");
    return super.isValidTarget(object);
  }

  getDamageRange() {
    const base = this.getDamageBase();
    const strengthBonus = Math.ceil(this.user.statistics.strength / 2);

    return [base + strengthBonus, 5];
  }

  triggerUseOn(target) {
    const targetScript = target.getScriptObject();

    switch (this.model.useMode) {
    case "hit":
      return super.triggerUseOn(target);
    case "mine":
      return { steps: this.getAnimationSteps(target), callback: targetScript.onMine.bind(targetScript, this.user) };
    }
    return false;
  }
}

export function create(model) {
  return new Pickaxe(model);
}