import {Explosion} from "./explosion.mjs";
import {getValueFromRange, randomCheck} from "../behaviour/random.mjs";

export function disarmAttempt(character, difficulty) {
  const skill = character.statistics.explosives;
  const roll = getValueFromRange(0, 100);
  var result = 0;

  randomCheck(skill - difficulty * 10, {
    criticalFailure: function() { result = -1; },
    failure:         function() { result = 0; },
    success:         function() { result = 1; }
  });
  return result;
}

export class Trap {
  constructor(model) {
    this.model = model;
    if (this.model.blocksPath)
      this.model.blocksPath = false;
    this.difficulty = 1;
    this.model.setAnimation(this.model.getVariable("disarmed") == true ? "off" : "on");
  }

  initialize() {
    this.model.toggleSneaking(true);
    this.model.interruptOnDetection = true;
  }

  getAvailableInteractions() {
    return ["look", "use-object", "use-skill"];
  }

  onDetected() {
    game.appendToConsole(i18n.t("messages.trap-detected"));
  }

  onLook() {
    const armedMessage = this.model.getVariable("disarmed") == true ? "inspection.disarmed" : "inspection.armed";

    game.appendToConsole(i18n.t("inspection.trap") + ' ' + i18n.t(armedMessage));
    return true;
  }

  onUseExplosives(user) {
    const result = disarmAttempt(user, this.difficulty);
    const disarmed = this.model.getVariable("disarmed") == true;

    if (result == -1) {
      game.appendToConsole(i18n.t("messages.trap-critical-failure"));
      this.model.setVariable("disarmed", false);
      this.triggered();
    }
    else if (result == 0)
      game.appendToConsole(!disarmed ? i18n.t("messages.trap-disarm-failed") : i18n.t("messages.trap-enable-failed"));
    else {
      game.appendToConsole(!disarmed ? i18n.t("messages.trap-disarmed") : i18n.t("messages.trap-armed"));
      this.model.setVariable("disarmed", !disarmed);
      this.model.setAnimation(disarmed ? "on" : "off");
    }
    return true;
  }

  onZoneExited() {
    if (this.model.getVariable("disarmed") != true)
      this.triggered();
  }

  triggered() {
    const explosion = new Explosion(this.model.position);

    this.model.setVariable("disarmed", true);
    explosion.withRadius(1)
             .withDamage(getValueFromRange(20, 50))
             .trigger();
    level.deleteObject(this.model);
  }
}

export function create(model) {
  return new Trap(model);
}
