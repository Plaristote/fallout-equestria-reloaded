import {toggleRoutine} from "./routine.mjs";

export class SceneManager {
  constructor(parent, id) {
    this.parent = parent;
    this.model = parent.model;
    this.id = id;
    if (this.active)
      this.triggerCurrentStep();
  }

  get storageScope() {
    return "scene-" + this.id;
  }

  get active() {
    return this.state !== null;
  }

  get state() {
    return this.model.hasVariable(this.storageScope)
      ? this.model.getVariable(this.storageScope)
      : null;
  }

  set state(value) {
    this.model.setVariable(this.storageScope, value);
  }

  get actors() {
    return [];
  }

  initialize() {
    this.state = 0;
    this.prepare();
  }

  prepare() {
    this.actors.forEach(actor => {
      toggleRoutine(actor, false);
      actor.getScriptObject().sceneManager = this;
      actor.actionQueue.reset();
    });
    this.triggerCurrentStep();
  }

  finalize() {
    this.actors.forEach(actor => {
      toggleRoutine(actor, true);
    });
    this.model.unsetVariable(this.storageScope);
  }

  triggerNextStep(continuing = true) {
    if (this.active) {
      this.state++;
      this.triggerCurrentStep(continuing);
    }
  }

  triggerCurrentStep(continuing = true) {
    this.states[this.state](continuing);
  }

  onCombatTurn(character) {
  }
}