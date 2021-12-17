import {getValueFromRange} from "../behaviour/random.mjs";
import {StackableBuff} from "./helpers/stackable.mjs";

const delay = 1800000;
const damageRanges = [
  [1,3],
  [2,4],
  [4,6],
  [5,7],
  [9,12]
];

class Poisoned extends StackableBuff {
  get poisonLevel() {
    return this.model.target.hasVariable("poisonLevel") ? this.model.target.getVariable("poisonLevel") : 1;
  }

  finalize() {
    this.model.target.unsetVariable("poisonLevel");
  }

  initialize() {
    console.log("Bleeding buff initialized");
    this.charges += 2;
    this.model.tasks.addTask("trigger", delay, 0);
  }

  repeat() {
    this.charges += 2;
  }

  trigger(times) {
    console.log("Poisoned buff triggered", this.poisonLevel);
    const rangeLevel = Math.min(Math.floor(this.poisonLevel / 3), damageRanges.length - 1);
    const damageRange = damageRanges[rangeLevel];
    const damage = this.poisonLevel + getValueFromRange(damageRange[0], damageRange[1]);

    this.charges -= times;
    this.model.target.takeDamage(damage * times, null);
    if (this.model.target === game.player) {
      game.appendToConsole(i18n.t("messages.poison-damage", {damage: damage * times}));
      game.sounds.play("notification");
    }
    if (this.charges <= 0)
      this.model.remove();
  }
}

export function create(model) {
  return new Poisoned(model);
}

