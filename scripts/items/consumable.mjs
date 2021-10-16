import {Item} from "./item.mjs";

export class Consumable extends Item {
  constructor(model) {
    super(model);
    this.useModes = ["use"];
  }

  get requiresTarget() {
    return !this.user || this.user.inventory.isEquippedItem(this.model);
  }

  getActionPointCost() {
    return 2;
  }

  isValidTarget(object) {
    return object.getObjectType() == "Character" && object.isAlive();
  }

  useOn(target) {
    if (!target)
      target = this.user;
    if (target.isAlive()) {
      this.consumedBy(target);
      this.user.inventory.destroyItem(this.model, 1);
      return true;
    }
    else
      game.appendToConsole(i18n.t("messages.invalid-target"));
    return false;
  }

  consumedBy(target) {
  }
}
