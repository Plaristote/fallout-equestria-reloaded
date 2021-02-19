export class CharacterBehaviour {
  constructor(model) {
    this.model = model;
    this.textBubbles = [
      { content: "Buck you filly", duration: 5000, color: "red" }
    ];
  }

  getAvailableInteractions() {
    const interactions = ["look", "use-skill"];

    if (this.dialog || this.textBubbles)
      interactions.unshift("talk-to");
  
    return interactions;
  }

  onLook() {
    game.appendToConsole("You see " + this.model.statistics.name);
  }

  onTalkTo() {
    if (this.dialog)
      level.initializeDialog(this.model, this.dialog);
    else
      this.displayRandomTextBubble();
  }

  displayRandomTextBubble() {
    const it = Math.floor(Math.random() * this.textBubbles.length);
    const textBubble = this.textBubbles[it];

    level.addTextBubble(this.model, textBubble.content, textBubble.duration, textBubble.color || "white");
  }

  onMovementEnded() {
  }

  onDestinationReached() {
    console.log(this.model, "reached destination", this.model.position.x, this.model.position.y);
  }
}

export function create(model) {
  return new CharacterBehaviour(model);
}
