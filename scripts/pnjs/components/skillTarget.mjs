import {MetabolismComponent} from "./metabolism.mjs";
import {getValueFromRange} from "../../behaviour/random.mjs";

export class SkillTargetComponent extends MetabolismComponent {
  constructor(model) {
    super(model);
  }

  onUseMedicine(user) {
    if (!this.model.isAlive())
      game.appendToConsole(i18n.t("messages.invalid-target"));
    else if (level.isInCombat(user))
      game.appendToConsole(i18n.t("medicine-skill.in-combat"));
    else
    {
      const stats   = this.model.statistics;
      const maxHeal = stats.maxHitPoints - stats.hitPoints;
      var   healed  = getValueFromRange(0, 10) + user.statistics.medicine / 10;
 
      healed = healed * user.statistics.medicine / 100;
      healed = Math.min(healed, maxHeal);
      stats.hitPoints += healed;
      game.appendToConsole(i18n.t("medicine-skill.used-on", {
        user:   user.statistics.name,
        target: stats.name,
        hp:     healed
      }));
      level.addTextBubble(this.model, "Thank you kindly !", 5000, "lightgreen");
      return true;
    }
    return false;
  }

  onTakeItem(user, item, quantity) {
    console.log("on take item", user, item, quantity);
    if (this.model.isAlive()) {
      level.addTextBubble(this.model, "Hey ! Don't touch that", 3000, "red");
      return false;
    }
    return true;
  }

  onPutItem(user, item, quantity) {
    if (this.model.isAlive()) {
    }
    console.log("on put item", user, item, quantity);
    return true;
  }
}
