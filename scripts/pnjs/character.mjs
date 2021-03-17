import {CombatComponent} from "./components/combat.mjs";

export class CharacterBehaviour extends CombatComponent {
  constructor(model) {
    super(model);
    this.xpValue = 25;
    this.textBubbles = [
      { content: "Lorem ipsum dolor sit amet, et toute ces sortes de choses.", duration: 5000, color: "yellow" }
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

    game.appendToConsole(i18n.t("inspection.character", {target: this.model.statistics.name}));
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
}

export function create(model) {
  return new CharacterBehaviour(model);
}
