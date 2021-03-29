import {Item} from "./item.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";
import {explode} from "../behaviour/explosion.mjs";
import {disarmAttempt} from "../behaviour/trap.mjs";

class Dynamite extends Item {
  constructor(model) {
    super(model);
  }

  getActionPointCost() {
    return 5;
  }

  get requiresTarget() {
    return false;
  }

  onLook() {
    const armedMessage = !this.model.tasks.hasTask("triggered") ? "inspection.disarmed" : "inspection.armed";
	  
    game.appendToConsole(i18n.t("inspection.item", {item: i18n.t("items." + this.model.itemType)}) + ' ' + i18n.t(armedMessage));
    return true;
  }

  useOn() {
    level.openCountdownDialog(this.model);
  }

  onCountdownReceived(timeout) {
    this.model.tasks.addTask("triggered", timeout * 1000, 1);
  }

  onUseExplosives(user) {
    const result = disarmAttempt(user, 2);

    if (result == -1) {
      game.appendToConsole(i18n.t("messages.trap-critical-failure"));
      this.triggered();
    }
    else if (result == 0)
      game.appendToConsole(i18n.t("messages.trap-disarm-failed"));
    else {
      game.appendToConsole(i18n.t("messages.trap-disarmed"));
      this.model.tasks.removeTask("triggered");
    }
    return true;
  }

  triggered() {
    const wearer   = this.model.getOwner();
    const radius   = 2;
    const position = wearer ? wearer.position : this.model.position;
    const damage   = getValueFromRange(20, 50);

    console.log("Exploding for", damage, "damage.");
    explode(position, radius, damage, wearer);
    if (wearer)
      wearer.inventory.destroyItem(this.model, 1);
    else
      level.deleteObject(this.model);
  }
}

export function create(model) {
  return new Dynamite(model);
}
