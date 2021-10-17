import {CharacterBehaviour} from "./character.mjs";

class TutoDog extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.speakOnDetection = true;
  }

  get dialog() {
    return this.model.hasVariable("questDone") ? null : "tutoDog";
  }

  get textBubbles() {
    return [
      { content: "Go away, pony !", duration: 3000, color: "red" },
      { content: "Get away from my gems !", duration: 3000, color: "red" }
    ];
  }
}

export function create(model) {
  console.log("CREATING TUTO DOG");
  return new TutoDog(model);
}

