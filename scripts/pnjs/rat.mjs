import {CharacterBehaviour} from "./character.mjs";
import {injectRoamTask} from "./tasks/roam.mjs";

export class Rat extends CharacterBehaviour {
  constructor(model) {
    super(model);
    injectRoamTask(this);
    this.prepareRoamTask(3);
  }

  getDefaultItem(slot) {
    return "melee-bite";
  }
}

export function create(model) {
  return new Rat(model);
}
