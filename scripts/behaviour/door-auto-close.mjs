import {Door} from "./door.mjs";

const autocloseDelay = 7500;
const autocloseMethod = "autoclose";

export class AutoClosingDoor extends Door {
  constructor(model) {
    super(model);
    if (!model.tasks.hasTask(autocloseMethod))
      model.tasks.addTask(autocloseMethod, autocloseDelay, 1);
  }

  autoclose() {
    if (!this.model.destroyed) {
      this.model.tasks.addTask(autocloseMethod, autocloseDelay, 1);
      this.model.opened = false;
    }
  }

  onUse() {
    if (this.model.tasks.hasTask(autocloseMethod)) {
      this.model.tasks.removeTask(autocloseMethod);
      this.model.tasks.addTask(autocloseMethod, autocloseDelay, 1);
    }
    return false;
  }
}

export function create(model) {
  return new AutoClosingDoor(model);
}
