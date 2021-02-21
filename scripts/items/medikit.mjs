import {Item} from "./item.mjs";

class Medikit extends Item {
  constructor(model) {
    super(model);
  }

  getActionPointCost() {
    return 5;
  }

  useOn(target) {
    const stats   = target.statistics;
    const maxHeal = stats.maxHitPoints - stats.hitPoints;
    var   healed  = this.getValueFromRange(15, 30);

    healed = healed * this.user.statistics.medicine / 100;
    healed = Math.min(healed, maxHeal);
    stats.hitPoints += healed;
    game.appendToConsole(this.user.statistics.name + " used " + this.model.objectName);
    game.appendToConsole(target.statistics.name + " was healed for " + healed + " hit points");
    return true;
  }
};

export function create(model) {
  return new Medikit(model);
}