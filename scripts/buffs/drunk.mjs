import {StackableBuff} from "./helpers/stackable.mjs";

const delay = 1800 * 1000;

class Drunk extends StackableBuff {
  constructor(model) {
    super(model);
    this.model.tasks.addTask("trigger", this.chargeDuration, 1);
  }

  get chargeDuration() {
    return delay * (this.hasTolerance ? 1 : 3);
  }

  get hasTolerance() {
    return this.model.target.statistics.traits.indexOf("chem-resistant") >= 0;
  }

  initialize() {
    super.initialize();
    this.updateModifiers();
  }
  
  repeat() {
    super.repeat();
    this.updateModifiers();
  }

  trigger(times) {
    this.charges -= times;
    this.updateModifiers();
    this.afterTrigger();
  }

  afterTrigger() {
    if (this.charges > 0)
      this.model.tasks.addTask("trigger", this.chargeDuration, 1);
    else
      this.model.remove();
  }

  updateModifier(statisticName, positive, limit) {
    const modifier        = Math.min(this.charges, limit);
    const currentModifier = this.model.hasVariable(statisticName) ? this.model.getVariable(statisticName) : 0;
    const diff            = modifier > currentModifier ? (modifier - currentModifier) : -(currentModifier - modifier);

    if (positive)
      this.model.target.statistics[statisticName] += diff;
    else
      this.model.target.statistics[statisticName] -= diff;
    this.model.setVariable(statisticName, modifier);
  }

  updateModifiers() {
    this.updateModifier("endurance",    true, 2);
    this.updateModifier("intelligence", false, 2);
  }
}

export function create(model) {
  return new Drunk(model);
}
