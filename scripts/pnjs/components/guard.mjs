export class GuardComponent {
  constructor(parent) {
    this.parent   = parent;
    this.model    = parent.model;
    this.taskName = "guardingTask";
    if (!this.model.hasVariable("guardX"))
      this.initialize();
    parent[this.taskName] = this.task.bind(this);
    if (!this.model.tasks.hasTask(this.taskName))
      this.model.tasks.addTask(this.taskName, 15000, 0);
  }

  initialize() {
    this.model.setVariable("guardX", this.model.position.x);
    this.model.setVariable("guardY", this.model.position.y);
  }

  task() {
    console.log("guardingTask");
    if (this.model.actionQueue.isEmpty() && !this.model.tasks.hasTask("alarmTask")) {
      this.model.movementMode = "walking";
      this.model.actionQueue.pushReachNear(
        this.model.getVariable("guardX"), this.model.getVariable("guardY"), 1
      );
      this.model.actionQueue.start();
    }
  }
}