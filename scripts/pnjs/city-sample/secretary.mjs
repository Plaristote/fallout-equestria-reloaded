import {CharacterBehaviour} from "../character.mjs";

class Secretary extends CharacterBehaviour {
  constructor(model) {
    super(model);
  }
}

export function create(model) {
  return new Secretary(model);
}
