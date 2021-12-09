function soonerFirst(a, b) { return a.nextTrigger < b.nextTrigger ? -1 : 1; }
function latestFirst(a, b) { return a.nextTrigger > b.nextTrigger ? -1 : 1; }
function randomInterval() { return Math.ceil(Math.random() * 3000); }

const refreshRoutineTaskName = "refreshRoutine";
const updateRoutineTaskName  = "updateRoutine";

export function toggleRoutine(character, value) {
  const script = character ? character.getScriptObject() : null;

  if (script && script.routineComponent) {
    if (value === undefined)
      script.routineComponent.interrupted = !script.routineComponent.interrupted;
    else
      script.routineComponent.interrupted = !value;
  }
}

function hasType(object, typeName) {
  return object.toString().startsWith(typeName + '(');
}

function isBusyCharacter(routine) {
  return level.isInCombat(routine.model) || !routine.model.actionQueue.isEmpty();
}

function isBusy(routine) {
  return routine.interrupted
      || (hasType(routine.model, "Character") && isBusyCharacter(routine));
}

function scheduleRoutineRefresh(routine, enabled = true) {
  const interval      = routine.refreshInterval + Math.ceil(Math.random() * 3000);
  const randomization = Math.ceil(Math.random() * 3000);

  routine.model.tasks.addTask(refreshRoutineTaskName, interval + randomization, 1);
}

function refreshRoutine(routine) {
  scheduleRoutineRefresh(routine);
  if (!isBusy(routine)) {
    const callback = routine.getCurrentRoutine().callback;
    routine.parent[callback]();
  }
}

export class RoutineComponent {
  constructor(parent, routine) {
    this.parent = parent;
    this.model = parent.model;
    this.routine = routine;
    this.refreshInterval = 4545;
    this.parent[updateRoutineTaskName] = this.updateRoutine.bind(this);
    this.parent[refreshRoutineTaskName] = () => refreshRoutine(this);
    if (!this.model.tasks.hasTask(updateRoutineTaskName))
      this.model.tasks.addTask(updateRoutineTaskName, randomInterval(), 1);
  }

  enablePersistentRoutine() {
    if (!this.model.tasks.hasTask(refreshRoutineTaskName))
      scheduleRoutineRefresh(this);
  }

  disablePersistentRoutine() {
    this.model.tasks.removeTask(refreshRoutineTaskName);
  }

  getRoutines() {
    const options = [];

    for (var i = 0 ; i < this.routine.length ; ++i) {
      const nextTrigger = game.timeManager.secondsUntilTime(this.routine[i]);

      options.push({ routine: this.routine[i], nextTrigger: parseInt(nextTrigger) * 1000 });
    }
    return options;
  }

  getCurrentRoutine() {
    const options = this.getRoutines().sort(latestFirst);
    return options[0].routine;
  }

  isActiveRoutine(name) {
    return this.getCurrentRoutine().callback === name;
  }

  scheduleNextRoutineAction() {
    const options = this.getRoutines().sort(soonerFirst);
    this.model.tasks.addTask(updateRoutineTaskName, options[0].nextTrigger, 1);
  }

  updateRoutine() {
    if (isBusy(this))
      this.model.tasks.addTask(updateRoutineTaskName, 1234, 1);
    else
      this.triggerRoutine();
  }

  triggerRoutine() {
    const callback = this.getCurrentRoutine().callback;

    this.scheduleNextRoutineAction();
    if (callback)
      this.parent[callback]();
  }
}
