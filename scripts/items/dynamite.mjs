//import {Item} from "./item.mjs";
import {ThrowableBehaviour} from "./throwable.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";
import {Explosion} from "../behaviour/explosion.mjs";
import {disarmAttempt} from "../behaviour/trap.mjs";

class Dynamite extends ThrowableBehaviour {
  constructor(model) {
    super(model);
    this.useModes = ["use", "throw"];
  }

  getActionPointCost() {
    switch (this.model.useMode) {
    case "throw":
      return 2;
    }
    return 5;
  }

  onLook() {
    const armedMessage = !this.model.tasks.hasTask("triggered") ? "inspection.disarmed" : "inspection.armed";

    game.appendToConsole(i18n.t("inspection.item", {item: this.trName}) + ' ' + i18n.t(armedMessage));
    return true;
  }

  isGroupable(other, defaultValue) {
    return this.model.tasks.hasTask("triggered") ? false : defaultValue;
  }

  useOn(target) {
    console.log("Using dynamite on", target);
    if (target == null) {
      level.openCountdownDialog(this.model);
      return true;
    }
    else
      game.appendToConsole(i18n.t("messages.nothing-happens"));
    return false;
  }

  onCountdownReceived(timeout) {
    const skill = this.user.statistics.explosives;
    const roll  = getValueFromRange(0, 100);

    if (roll >= 99) {
      game.appendToConsole(i18n.t("messages.explosive-critical-failure"));
      this.triggered();
      return ;
    }
    else if (roll >= 95 || roll >= skill) {
      var difference = getValueFromRange(0, timeout / 2);
      var direction = getValueFromRange(0, 100);

      timeout += direction <= 50 ? -difference : difference;
      this.model.setVariable("trigger-failed", true);
    }
    else
      this.model.setVariable("trigger-failed", false);
    this.scheduleTrigger(timeout);
    if (this.model.quantity > 1)
      this.splitFromInactiveItems();
  }

  splitFromInactiveItems() {
    const itemsToRecreate = this.model.quantity - 1;

    this.model.quantity = 1;
    this.model.getOwner().inventory.addItemOfType(this.model.itemType, itemsToRecreate);
  }

  scheduleTrigger(timeout) {
    this.model.tasks.addTask("beforeTriggered", (timeout - 3) * 1000, 1);
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
      this.model.tasks.removeTask("beforeTriggered");
      this.model.tasks.removeTask("triggered");
    }
    return true;
  }

  beforeTriggered() {
    const wearer = this.model.getOwner();

    if (wearer)
      level.addTextBubble(wearer, "Where's that ticking sound coming from ?", 3000, "white");
  }

  triggered() {
    const wearer    = this.model.getOwner();
    const position  = wearer ? wearer.position : this.model.position;
    const explosion = new Explosion(position);

    explosion.withRadius(2)
             .withDamage(getValueFromRange(20, 50))
             .withWearer(wearer)
             .trigger();
    if (this.model.getVariable("trigger-failed") == true)
      game.appendToConsole(i18n.t("messages.explosive-triggered-prematurely", { item: this.trName }));
    else
      game.appendToConsole(i18n.t("messages.explosive-triggered", { item: this.trName }));
    if (wearer)
      wearer.inventory.destroyItem(this.model, 1);
    else
      level.deleteObject(this.model);
  }
}

export function create(model) {
  return new Dynamite(model);
}
