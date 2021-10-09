class Ko {
  construct(model) {
    this.model = model;
  }

  get charges() {
    return this.model.getVariable("charges");
  }

  set charges(value) {
    this.model.setVariable("charges", value);
  }

  initialize() {
    this.charges = 1;
    this.model.target.fallUnconscious();
    game.appendToConsole(i18n.t("messages.ko", {
      target: this.model.target.statistics.name
    }));
    this.model.tasks.addTask("wakeUp", 10000, 0);
  }

  repeat() {
    this.charges = this.charges + 1;
  }

  trigger() {
    this.charges = this.charges - 1;
    if (this.charges <= 0) {
      this.model.remove();
      this.model.target.wakeUp();
    }
  }
}

export function create(model) {
  return new Ko(model);
}
