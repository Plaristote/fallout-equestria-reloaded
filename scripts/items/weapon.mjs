import {Item} from "./item.mjs";

export class Weapon extends Item {
  constructor(model) {
    super(model);
    this.triggersCombat = true;
  }

  isValidTarget(object) {
    return object.getObjectType() === "Character" && object !== this.user;
  }

  useOn(target) {
    var damage = this.getValueFromRange(this.getDamageRange());

    damage -= target.statistics.damageResistance;
    damage = Math.max(0, dmage);
    game.appendToConsole(
      this.user.statistics.name +
      " used " +
      this.model.objectName +
      " on " +
      target.statistics.name +
      " for " +
      damage + " damage."
    );
    target.statistics.hitPoints -= damage;
    return true;
  }
}
