import {CombatComponent} from "./components/combat.mjs";

export class CharacterBehaviour extends CombatComponent {
  constructor(model) {
    super(model);
    this.xpValue = 25;
    this.canPush = true;
  }

  getAvailableInteractions() {
    if (this.model.isAlive()) {
      const interactions = ["look", "use-object", "use-skill"];

      if (!level.combat) {
        if (this.canPush)
          interactions.unshift("push");
        if (this.dialog || this.textBubbles)
          interactions.unshift("talk-to");
      }
      return interactions;
    }
    return ["use", "look"];
  }

  onLook() {
    game.appendToConsole(i18n.t("inspection.character", {target: this.model.statistics.name}));
    return true;
  }

  onPush() {
    this.model.moveAway();
  }

  onTalkTo() {
    if (this.dialog)
      level.initializeDialog(this.model, this.dialog);
    else
      this.displayRandomTextBubble();
  }

  displayRandomTextBubble() {
    if (this.textBubbles && this.textBubbles.length > 0) {
      const it = Math.floor(Math.random() * this.textBubbles.length);
      const textBubble = this.textBubbles[it];

      level.addTextBubble(this.model, textBubble.content, textBubble.duration, textBubble.color || "white");
    }
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
