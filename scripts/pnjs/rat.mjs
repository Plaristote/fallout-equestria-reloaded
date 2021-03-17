import {CharacterBehaviour} from "./character.mjs";

class Rat extends CharacterBehaviour {
  getDefaultItem(slot) {
    return "melee-bite";
  }
}

export function create(model) {
  return new Rat(model);
}
