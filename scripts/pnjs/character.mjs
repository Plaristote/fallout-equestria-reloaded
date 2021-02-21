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
    var message = "You see " + this.model.statistics.name + ".";

    message += " " + this.model.statistics.hitPoints + "/" + this.model.statistics.maxHitPoints + " HP";
    game.appendToConsole(message);
    return true;
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

  onTurnStart() {
    console.log("Lol, I dunno what to do, skipping turn");
    level.passTurn(this.model);
  }
}

export function create(model) {
  return new CharacterBehaviour(model);
}
