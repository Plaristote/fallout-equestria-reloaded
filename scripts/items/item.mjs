import {areInContact} from "../behaviour/pathfinding.mjs";

export class Item {
  constructor(model) {
    this.model = model;
    this.triggersCombat = false;
    this.isValidTarget = function() { return true; };
  }

  onEquipped(character, on) {
    console.log("ON EQUIPED ITEM MAGGLE", character, on);
    this.user = on ? character : null;
  }

  getActionPointCost() {
    return 2;
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
    console.log("Attempt to uze on", this, this.isValidTarget, this.model, this.triggersCombat, this.getActionPointCost);
    if (this.isValidTarget(target)) {
      if (this.isInRange(target)) {
        if (this.user.useActionPoints(this.getActionPointCost())) {
          return {
            steps:    this.getAnimationSteps(target),
            callback: this.useOn.bind(this, target)
          };
	}
        else
          this.logFailure("Not enough action points.");
      }
      else
        this.logFailure("Out of range.");
    }
    else
      this.logFailure("Invalid target");
    return false;
  }

  useOn(target) {
    this.logFailure("That does nothing.");
  }

  logFailure(message) {
    //if (this.user == level.player)
      game.appendToConsole(message);
  }
}
