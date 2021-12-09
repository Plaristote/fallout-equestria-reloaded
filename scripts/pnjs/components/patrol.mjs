export const PatrolMode = {
  Linear: 0,
  Random: 1
};

export class PatrolComponent {
  constructor(parent) {
    this.parent   = parent;
    this.model    = parent.model;
    this.taskName = "patrolTask";
    this.mode     = PatrolMode.Linear;
    this.enable();
    parent[this.taskName] = this.task.bind(this);
  }

  get patrolStep() {
    console.log("getPatrolZtep");
    return this.model.hasVariable("patrolStep")
      ? this.model.getVariable("patrolStep")
      : 0;
  }

  set patrolStep(value) { this.model.setVariable("patrolStep", value); }

  getNextPatrolPoint() {
    if (this.mode == PatrolMode.Random)
      return Math.floor(Math.random() * 100) % this.patrolSpots.length;
    this.patrolStep = (this.patrolStep + 1) % this.patrolSpots.length;
    return this.patrolStep;
  }

  task() {
    if (this.model.actionQueue.isEmpty() && !this.model.tasks.hasTask("alarmTask")) {
      const i = this.getNextPatrolPoint();

      this.model.movementMode = "walking";
      this.model.actionQueue.pushReachNear(this.patrolSpots[i].x, this.patrolSpots[i].y, 3);
      this.model.actionQueue.start();
    }
  }

  enable() {
    if (!this.model.tasks.hasTask(this.taskName))
      this.model.tasks.addTask(this.taskName, 15000, 0);
  }

  disable() {
    this.model.tasks.removeTask(this.taskName);
  }
}