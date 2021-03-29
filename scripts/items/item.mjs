import {areInContact} from "../behaviour/pathfinding.mjs";

export class Item {
  constructor(model) {
    this.model = model;
    this.triggersCombat = false;
    this.useModes = ["use"];
  }

  get user() {
    return this.model.getOwner();
  }

  onLook() {
    game.appendToConsole(i18n.t("inspection.item", {item: i18n.t("items." + this.model.itemType)}));
    return true;
  }

  onEquipped(character, on) {
    console.log("ON EQUIPED ITEM MAGGLE", this.model, character, on);
  }

  getActionPointCost() {
    return 2;
  }

  get requiresTarget() {
    return true;
  }

  isValidTarget(object) {
    return true;
  }

  isInRange(object) {
    if (this.user) {
      const pos1  = this.user.position;
      const pos2  = object.position;

      // TODO implement items with actual range
      return areInContact(pos1, pos2);
    }
    return false;
  }

  getAnimationSteps(target) {
    return [{ type: "Animation", animation: "use", object: this.user }];
  }

  attemptToUseOn(target) {
    if (this.isValidTarget(target)) {
      if (this.isInRange(target)) {
        if (this.user.useActionPoints(this.getActionPointCost()))
          return this.triggerUseOn(target);
        else
          this.logFailure(i18n.t("messages.not-enough-ap"));
      }
      else
        this.logFailure(i18n.t("messages.out-of-range"));
    }
    else
      this.logFailure(i18n.t("messages.invalid-target"));
    console.log("Wait wtf?", this.user, level.player);
    return false;
  }

  triggerUseOn(target) {
    return {
      steps:    this.getAnimationSteps(target),
      callback: this.useOn.bind(this, target)
    };
  }

  useOn(target) {
    this.logFailure("That does nothing.");
  }

  logFailure(message) {
    if (this.user == level.player)
      game.appendToConsole(message);
  }
}
