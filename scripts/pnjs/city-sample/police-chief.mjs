import {CharacterBehaviour} from "../character.mjs";

class PoliceChief extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.textBubbles = [
      {content: "I'm busy. Leave and close the door behind you.", duration: 3500, color: "yellow"},
      {content: "Now isn't a good time. Scram.", duration: 3000, color: "yellow"},
      {content: "You've got nothing to do here. Leave my office this instant.", duration: 3500, color: "yellow"}
    ];
  }
}

export function create(model) {
  return new PoliceChief(model);
}
