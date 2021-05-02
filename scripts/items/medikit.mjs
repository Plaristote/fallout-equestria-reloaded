import {Item} from "./item.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

export class Medikit extends Item {
  constructor(model) {
    super(model);
    this.useModes = ["use"];
    this.minHeal = 15;
    this.maxHeal = 30;
  }

  getActionPointCost() {
    return 5;
  }

  useOn(target) {
    if (target.isAlive()) {
      const stats   = target.statistics;
      const maxHeal = stats.maxHitPoints - stats.hitPoints;
      var   healed  = getValueFromRange(this.minHeal, this.maxHeal);

      healed = healed * this.user.statistics.medicine / 100;
      healed = Math.min(healed, maxHeal);
      healed = Math.floor(healed);
      stats.hitPoints += healed;
      game.appendToConsole(i18n.t("messages.use", {
        user: this.user.statistics.name, item: i18n.t("items." + this.model.itemType)
      }));
      game.appendToConsole(i18n.t("messages.healed", {
        target: target.statistics.name, hp: healed
      }));
      return true;
    }
    else
      game.appendToConsole(i18n.t("messages.invalid-target"));
    return false;
  }
};

export function create(model) {
  return new Medikit(model);
}
