import {MovementComponent} from "./movement.mjs";

export class MetabolismComponent extends MovementComponent {
  constructor(model) {
    super(model);
    if (!this.model.hasVariable("metabolismEnabled")) {
      this.metabolismState = 0;
      this.model.setVariable("metabolismEnabled", true);
      this.model.tasks.addTask("updateMetabolism", 86400000 / 24, 0);
    }
  }

  get isResting() { return this.model.getVariable("resting") == true; }
  get metabolismState() { return this.model.getVariable("metabolismState"); }
  set metabolismState(value) { this.model.setVariable("metabolismState", value); }

  updateMetabolism(count) {
    if (this.model.isAlive()) {
      console.log("Update metabolism", count);
      const periodicity  = this.isResting ? 2 : 24;
      const ticks        = Math.floor((count + this.metabolismState) / periodicity);
      const remains      = (count + this.metabolismState) % periodicity;
      const maxHitPoints = this.model.statistics.maxHitPoints;
      const healingRate  = this.model.statistics.healingRate;
      const currentHp    = this.model.statistics.hitPoints;
      
      this.metabolismState = remains;
      this.model.statistics.hitPoints = Math.min(currentHp + (healingRate * ticks), maxHitPoints);
      return true;
    }
    return false;
  }
}
