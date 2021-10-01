import {MetabolismComponent} from "./metabolism.mjs";
import {getValueFromRange} from "../../behaviour/random.mjs";
import {skillCheck} from "../../cmap/helpers/checks.mjs";

export class DialogComponent extends MetabolismComponent {
  constructor(model) {
    super(model);
  }

  onTalkTo() {
    this.model.setVariable("talkedTo", 1);
    if (this.dialog)
      level.initializeDialog(this.model, this.dialog);
    else
      this.displayRandomTextBubble();
    return true;
  }

  displayRandomTextBubble(options) {
    if (!options) { options = this.textBubbles; }
    if (options && options.length > 0) {
      const it = Math.floor(Math.random() * options.length);
      const textBubble = options[it];

      level.addTextBubble(this.model, textBubble.content, textBubble.duration, textBubble.color || "white");
    }
  }

  dialogDetectionHook() {
    if (this.speakOnDetection) {
      game.player.fieldOfView.setCharacterDetected(this.model);
      game.player.toggleSneaking(false);
      this.onTalkTo();
      return true;
    }
    return false;
  }
}
