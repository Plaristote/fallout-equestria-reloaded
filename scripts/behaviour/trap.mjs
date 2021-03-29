import {explode} from "./explosion.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

export function disarmAttempt(character, difficulty) {
  const skill = character.statistics.explosives;
  const roll = getValueFromRange(0, 100);

  console.log("Roll=", roll, ", threshold=", skill + difficulty * 10);
  if (skill - difficulty * 10 >= roll)
    return 1;
  else if (roll >= 95)
    return -1;
  return 0;
}

export class Trap {
  constructor(model) {
    this.model = model;
    if (this.model.blocksPath)
      this.model.blocksPath = false;
    this.difficulty = 1;
  }

  getAvailableInteractions() {
    return ["look", "use-object", "use-skill"];
  }

  onLook() {
    const armedMessage = this.model.getVariable("disarmed") == true ? "inspection.disarmed" : "inspection.armed";

    game.appendToConsole(i18n.t("inspection.trap") + ' ' + i18n.t(armedMessage));
    return true;
  }

  onUseExplosives(user) {
    const result = disarmAttempt(user, this.difficulty);

    if (result == -1) {
      game.appendToConsole(i18n.t("messages.trap-critical-failure"));
      this.model.setVariable("disarmed", false);
      this.triggered();
    }
    else if (result == 0)
      game.appendToConsole(i18n.t("messages.trap-disarm-failed"));
    else {
      game.appendToConsole(i18n.t("messages.trap-disarmed"));
      this.model.setVariable("disarmed", true);
    }
    return true;
  }

  onZoneExited() {
    if (this.model.getVariable("disarmed") != true)
      this.triggered();
  }

  triggered() {
    this.model.setVariable("disarmed", true);
    explode(this.model.position, 1, getValueFromRange(20, 50));
  }
}

export function create(model) {
  return new Trap(model);
}
