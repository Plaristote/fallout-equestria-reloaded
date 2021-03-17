import {CharacterBehaviour} from "./character.mjs";

class Player extends CharacterBehaviour {
  constructor(model) {
    super(model);
  }

  getAvailableInteractions() {
    return [];
    return super.getAvailableInteractions(); // TODO
  }

  onTurnStart() {
  }

  onActionQueueCompleted() {
  }

  getDefaultItem(slot) {
    switch (slot) {
    case "use-1":
      return "melee-hoofercut";
    case "use-2":
      return "melee-bucking";
    }
    return "melee";
  }
}

export function create(model) {
  return new Player(model);
}
