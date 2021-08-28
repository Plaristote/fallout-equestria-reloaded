import {randomInterval} from "../../behaviour/random.mjs";

class RoamTask {
  constructor(parent) {
    this.parent = parent;
    this.model = parent.model;
  }

  prepare(minInterval, maxInterval) {
    this.model.setVariable("roamX", this.model.position.x);
    this.model.setVariable("roamY", this.model.position.y);
    this.interval(minInterval, maxInterval);
  }

  start() {
    this.model.setVariable("roamEnabled", 1);
    this.schedule();
  }

  terminate() {
    this.model.unsetVariable("roamEnabled");
    this.model.tasks.removeTask("roamTask");
  }

  range(value) {
    this.model.setVariable("roamRange", value);
  }

  interval(min, max) {
    this.model.setVariable("roamMin", min);
    this.model.setVariable("roamMax", max);
  }

  schedule() {
    const minInterval = this.model.getVariable("roamMin");
    const maxInterval = this.model.getVariable("roamMax");
    const interval    = randomInterval(minInterval * 1000, maxInterval * 1000);

    console.log("ZCHEDULING", minInterval, maxInterval, interval);
    this.model.tasks.addTask("_roamTask", Math.floor(interval), 1);
    console.log("ZCHEDULING#2");
  }

  exists() {
    return this.model.hasVariable("roamEnabled");
  }

  run() {
    console.log("COUCOU #1");
    if (this.exists()) {
      console.log("COUCOU #2");
      if (!level.isInCombat(this.model) && this.model.actionQueue.isEmpty()) {
        console.log("COUCOU #3");
        const range = this.model.getVariable("roamRange");
        const center = {
          x: this.model.getVariable("roamX"),
          y: this.model.getVariable("roamY")
        };

        console.log("ROAM TAZK PUZH NEAR", range, center.x, center.y);
        this.model.actionQueue.pushReachNear(center.x, center.y, range);
        this.model.actionQueue.start();
      }
      this.schedule();
    }
  }
}

function prepareRoamTask(range, minInterval = 5, maxInterval = 15) {
  this.model.setVariable("roamRange", range);
  this.roamTask = new RoamTask(this);
  if (!this.roamTask.exists()) {
    this.roamTask.prepare(range, minInterval, maxInterval);
    this.roamTask.start();
  }
  else {
    this.roamTask.range(range);
    this.roamTask.interval(minInterval, maxInterval);
  }
}

function roamTask() {
  console.log("ROAM TAZK TRIGGER", this.roamTask, this.roamTask.run);
  this.roamTask.run();
}


export function injectRoamTask(object) {
  object.prepareRoamTask =  prepareRoamTask.bind(object);
  object._roamTask = roamTask.bind(object);
}