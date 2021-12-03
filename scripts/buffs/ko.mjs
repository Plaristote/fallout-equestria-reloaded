import {StackableBuff} from "./helpers/stackable.mjs";

class Ko extends StackableBuff {
  initialize() {
    this.charges = 1;
    this.model.target.fallUnconscious();
    game.appendToConsole(i18n.t("messages.ko", {
      target: this.model.target.statistics.name
    }));
    this.model.tasks.addTask("trigger", 10000, 0);
  }

  trigger() {
    this.charges--;
    if (this.charges <= 0)
      this.model.remove();
  }
  
  finalize() {
    this.model.target.wakeUp();
  }
}

export function create(model) {
  return new Ko(model);
}
