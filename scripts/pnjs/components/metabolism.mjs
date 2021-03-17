export class MetabolismComponent {
  constructor(model) {
    this.model = model;
    if (!this.model.hasVariable("metabolismEnabled"))
    {
      this.model.setVariable("metabolismEnabled", true);
      this.model.tasks.addTask("updateMetabolism", 86400000, 0);
    }
  }

  updateMetabolism() {
    if (this.model.isAlive()) {
      const maxHitPoints = this.model.statistics.maxHitPoints;
      const healingRate  = this.model.statistics.healingRate;
      const currentHp    = this.model.statistics.hitPoints;
      
      this.model.statistics.hitPoints = Math.min(currentHp + healingRate, maxHitPoints);
    }
  }
}
