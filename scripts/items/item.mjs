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

  getValueFromRange(from, to) {
    return from + Math.floor(Math.random() * (to + 1));
  }

  getActionPointCost() {
    return 2;
  }

  isValidTarget(object) {
    return true;
  }

  isInRange(object) {
    console.log("IzInRange", this.user, object);
    if (this.user) {
      const pos1  = this.user.position;
      const pos2  = object.position;
      const distX = Math.max(pos1.x, pos2.x) - Math.min(pos1.x, pos2.y);
      const distY = Math.max(pos1.y, pos2.y) - Math.min(pos1.y, pos2.y);

      console.log("Diztance:", pos1, pos2, distX, distY);
      return distX <= 1 && distY <= 1;
    }
    return false;
  }

  attemptToUseOn(target) {
    console.log("Attempt to uze on", this, this.isValidTarget, this.model, this.triggersCombat, this.getActionPointCost);
    if (this.isValidTarget(target)) {
      if (this.isInRange(target)) {
        if (this.user.useActionPoints(this.getActionPointCost()))
          return this.useOn(target);
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
