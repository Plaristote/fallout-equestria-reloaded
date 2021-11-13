export const AlarmLevel = {
  ShootOnSight: 0,
  Arrest: 1
};

export function callGuards(guards, target, alarmLevel) {
  console.log("Calling guards", guards, guards.objects.length,
    "to position", target.position.x, target.position.y, target);
  for (var i = 0 ; i < guards.groups.length ; ++i)
    callGuards(guards.groups[i], target, alarmLevel);
  for (var i = 0 ; i < guards.objects.length ; ++i) {
    guards.objects[i].getScriptObject().receiveAlarmSignal(
      target.position.x, target.position.y, target, alarmLevel
    );
  }
}

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
  get alarmLevel() { return this.model.getVariable("alarmLevel"); }
  set alarmLevel(value) { return this.model.setVariable("alarmLevel", value); }
  get targetPath() { return this.model.getVariable("alarmTarget"); }
  set targetPath(value) { this.model.setVariable("alarmTarget", value); }
  get target() { return level.findObject(this.targetPath); }
  get isActive() { return this.model.tasks.hasTask("alarmTask"); }

  alarmTask() {
    if (this.model.actionQueue.isEmpty())
      this.goToAlarmSignal();
  }
  
  receiveAlarmSignal(x, y, target, alarmLevel) {
    console.log("Guard", this.model, "received alarm signal", x, y, alarmLevel);
    this.configureAlarmLevel(target, alarmLevel);
    if (!this.model.fieldOfView.isDetected(target))
    {
      this.model.movementMode = "running";
      this.alarmPosition = { x: x, y: y };
      this.goToAlarmSignal();
      this.model.tasks.addTask("alarmTask", 1500, 0);
    }
    else
      this.onTargetFound();
  }

  configureAlarmLevel(target, alarmLevel) {
    if (this.model.hasVariable("alarmLevel"))
      this.alarmLevel = Math.min(alarmLevel, this.alarmLevel);
    else
      this.alarmLevel = alarmLevel;
    this.targetPath = target ? target.path : level.player.path;
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
  
  isAlarmSignalReached() {
    if (this.model.hasVariable("alarmAtX"))
      return this.model.getDistance(this.alarmPosition.x, this.alarmPosition.y) <= 2;
    return false;
  }
  
  cancelAlarmSignal() {
    this.onAlarmSignalReached();
    this.model.actionQueue.reset();
  }

  onAlarmSignalReached() {
    this.model.unsetVariable("alarmAtX");
    this.model.unsetVariable("alarmAtY");
    this.model.unsetVariable("alarmTarget");
    this.model.unsetVariable("alarmLevel");
    this.model.tasks.removeTask("alarmTask");
  }
  
  onTargetFound() {
    if (this.alarmLevel === AlarmLevel.Arrest && this.targetPath === "player")
      level.initializeDialog(this.model, "town-guard");
    else {
      this.model.setAsEnemy(this.target);
      this.model.requireJoinCombat();
    }
    this.onAlarmSignalReached();
  }

  onCharacterDetected(character) {
    if (character.path === this.targetPath && this.isActive) {
      this.onTargetFound();
      return true;
    }
    return false;
  }
  
  onTurnStart() {
    if (!this.parent.findCombatTarget() && this.isActive) {
      this.goToAlarmSignal();
      return true;
    }
    return false;
  }
  
  onActionQueueCompleted() {
    if (this.isAlarmSignalReached()) {
      this.model.fieldOfView.detectCharacters();
      this.onAlarmSignalReached();
    }
  }
}
