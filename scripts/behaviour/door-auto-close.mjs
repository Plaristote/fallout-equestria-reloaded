import {Door} from "./door.mjs";

export class AutoClosingDoor extends Door {
  constructor(model) {
    super(model);
    if (!model.tasks.hasTask("autoclose"))
      model.tasks.addTask("autoclose", 7500, 1);
  }

  autoclose() {
    if (!this.model.destroyed) {
      this.model.tasks.addTask("autoclose", 7500, 1);
      this.model.opened = false;
    }
  }
}

export function create(model) {
  return new AutoClosingDoor(model);
}
