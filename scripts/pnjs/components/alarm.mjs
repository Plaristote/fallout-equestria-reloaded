export class AlarmComponent {
  constructor(parent) {
    this.parent = parent;
    this.parent.alarmTask = this.alarmTask.bind(this);
    this.parent.receiveAlarmSignal = this.receiveAlarmSignal.bind(this);
  }

  get model() { return this.parent.model; }
  get alarmPosition() {
    return {
      x: this.model.getVariable("alarmAtX"),
      y: this.model.getVariable("alarmAtY")
    };
  }
  set alarmPosition(value) {
    this.model.setVariable("alarmAtX", value.x);
    this.model.setVariable("alarmAtY", value.y);
  }
  get targetPath() { return this.model.getVariable("alarmTarget"); }
  set targetPath(value) { this.model.setVariable("alarmTarget", value); }
  get isActive() { return this.model.tasks.hasTask("alarmTask"); }

  alarmTask() {
    if (this.model.actionQueue.isEmpty())
      this.goToAlarmSignal();
  }
  
  receiveAlarmSignal(x, y, target) {
    console.log("Guard", this.model, "received alarm signal", x, y);
    this.model.movementMode = "running";
    this.alarmPosition = { x: x, y: y };
    this.targetPath = target ? target.path : level.player.path;
    this.goToAlarmSignal();
    this.model.tasks.addTask("alarmTask", 1500, 0);
  }

  goToAlarmSignal() {
    const target = this.alarmPosition;

    this.model.actionQueue.reset();
    this.model.actionQueue.pushReachNear(target.x, target.y, 2);
    if (this.model.actionQueue.start())
      console.log(this.model, "going toward alarm", target.x, target.y);
    else
      console.log(this.model, "goToArlamSignal failed");
  }
  
  iaAlarmSignalReached() {
    if (this.model.hasVariable("alarmAtX"))
      return this.model.getDistance(this.alarmPosition.x, this.alarmPosition.y) <= 2;
    return false;
  }

  onAlarmSignalReached() {
    this.model.unsetVariable("alarmAtX");
    this.model.unsetVariable("alarmAtY");
    this.model.unsetVariable("alarmTarget");
    this.model.tasks.removeTask("alarmTask");
  }

  onCharacterDetected(character) {
    if (character.path === this.targetPath && this.isActive) {
      this.onAlarmSignalReached();
      this.model.setAsEnemy(character);
      if (!level.isInCombat(this.model))
        level.joinCombat(this.model);
    }
  }
  
  onTurnStart() {
    if (!this.parent.findCombatTarget() && this.isActive) {
      this.goToAlarmSignal();
      return true;
    }
    return false;
  }
  
  onActionQueueCompleted() {
    if (this.iaAlarmSignalReached())
      this.onAlarmSignalReached();
  }
}
