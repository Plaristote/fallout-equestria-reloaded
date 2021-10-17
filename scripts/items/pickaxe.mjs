import {MeleeAttack} from "./melee.mjs";

export class Pickaxe extends MeleeAttack {
  constructor(model) {
    super(model);
    this.useModes.push("mine");
    this.skill = "meleeWeapons";
  }

  get triggersCombat() {
    return this.model.useMode === "hit";
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
    switch (this.model.useMode) {
    case "hit":
      return super.triggerUseOn(target);
    case "mine":
      return { steps: this.getUseAnimation(), callback: this.onMine.bind(this, target) };
    }
    return false;
  }

  onMine(target) {
    const quantity = Math.ceil(Math.random() * 5);
    var mineCount = 0;

    if (target.hasVariable("mineCount"))
      mineCount = target.getVariable("mineCount");
    if (mineCount < 3)
    {
      game.appendToConsole(i18n.t("messages.collected-ore", { quantity: quantity }));
      this.user.inventory.addItemOfType("gemstone", quantity);
      target.setVariable("mineCount", mineCount + 1);
    }
    else
      game.appendToConsole(i18n.t("messages.collected-ore-failure"));
  }
}

export function create(model) {
  return new Pickaxe(model);
}