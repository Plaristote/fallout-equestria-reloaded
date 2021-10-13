function soonerFirst(a, b) { return a.nextTrigger < b.nextTrigger ? -1 : 1; }
function latestFirst(a, b) { return a.nextTrigger > b.nextTrigger ? -1 : 1; }
function randomInterval() { return Math.ceil(Math.random() * 3000); }

export class RoutineComponent {
  constructor(parent, routine) {
    this.parent = parent;
    this.model  = parent.model;
    this.routine = routine;
    this.parent.updateRoutine = this.updateRoutine.bind(this);
    if (!this.model.tasks.hasTask("updateRoutine"))
      this.model.tasks.addTask("updateRoutine", randomInterval(), 1);
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

  scheduleNextRoutineAction() {
    const options = this.getRoutines().sort(soonerFirst);
    this.model.tasks.addTask("updateRoutine", options[0].nextTrigger, 1);
  }

  updateRoutine() {
    this.scheduleNextRoutineAction();
    this.parent[this.getCurrentRoutine().callback]();
  }
}
