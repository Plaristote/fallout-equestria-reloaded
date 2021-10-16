import {Consumable} from "./consumable.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

class HealthPotion extends Consumable {
  constructor(model) {
    super(model);
    this.minHeal = 12;
    this.maxHeal = 27;
  }

  consumedBy(target) {
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
  }
}

export function create(model) {
  return new HealthPotion(model);
}
