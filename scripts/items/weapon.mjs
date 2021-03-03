import {Item} from "./item.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

export class Weapon extends Item {
  constructor(model) {
    super(model);
    this.triggersCombat = true;
  }

  isValidTarget(object) {
    return object.getObjectType() === "Character" && object !== this.user;
  }

  useOn(target) {
    var damage = getValueFromRange(...this.getDamageRange());

    damage -= target.statistics.damageResistance;
    damage = Math.max(0, damage);
    game.appendToConsole(
      this.user.statistics.name +
      " used " +
      this.model.objectName +
      " on " +
      target.statistics.name +
      " for " +
      damage + " damage."
    );
    target.takeDamage(damage, this.user);
    return true;
  }
}
