import {getValueFromRange} from "../behaviour/random.mjs";

class Irradiated {
  constructor(model) {
    this.model = model;
  }

  get target() {
    return this.model.target;
  }
	
  get charges() {
    return this.model.getVariable("charges");
  }

  set charges(value) {
    this.model.setVariable("charges", value);
  }

  consoleMessage(message) {
    if (this.target === level.player)
      game.appendToConsole(message);
  }

  initialize() {
    this.charges = 1;
    this.model.tasks.addTask("trigger", 3600000, 0);
    this.model.tasks.addTask("wearOff", 86400000, 0);
    this.consoleMessage(i18n.t("messages.radiation-begin"));
  }

  restart() {
    this.charges = this.charges + 1;
  }

  trigger() {
    game.appendToConsole(`Coucou: ${this.charges}`);
    const resistance = this.target.statistics.radiationResistance;
    var damage       = getValueFromRange(this.charges, this.charges * 1.5);

    damage = damage * (resistance / 100);
    damage = Math.ceil(damage);
    this.target.takeDamage(damage, null);
    this.consoleMessage(i18n.t("messages.damaged", {
      target: this.target.statistics.name,
      damage: damage
    }));
  }

  wearOff() {
    game.appendToConsole("Adieu");
    this.charges = this.charges - 1;
    if (this.charges <= 0) {
      this.model.remove();
      game.appendToConsole(i18n.t("messages.radiation-over"));
    }
  }
}

export function create(model) {
  return new Irradiated(model);
}
