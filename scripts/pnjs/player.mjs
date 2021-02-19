import {CharacterBehaviour} from "./character.mjs";

class Player extends CharacterBehaviour {
  constructor(model) {
    super(model);
  }

  getAvailableInteractions() {
    return [];
    return super.getAvailableInteractions(); // TODO
  }
}

export function create(model) {
  return new Player(model);
}
