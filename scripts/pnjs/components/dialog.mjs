import {MetabolismComponent} from "./metabolism.mjs";
import {getValueFromRange} from "../../behaviour/random.mjs";
import {skillCheck} from "../../cmap/helpers/checks.mjs";

function isInAutoTalkRange(character) {
  return character.getDistance(game.player) <= 10;
}

function canAutoTalk(character) {
  return !level.combat && isInAutoTalkRange(character)
}

export class DialogComponent extends MetabolismComponent {
  constructor(model) {
    super(model);
  }

  onTalkTo() {
    const dialog = this._delayedDialog || this.dialog;

    this.model.setVariable("talkedTo", 1);
    delete this._delayedDialog;
    if (dialog)
      level.initializeDialog(this.model, dialog);
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

  startDialog(dialog = null) {
    game.player.fieldOfView.setCharacterDetected(this.model);
    this.model.fieldOfView.setCharacterDetected(game.player);
    this.model.tasks.addUniqueTask("onTalkTo", 600, 1);
    this._delayedDialog = dialog;
    level.cameraFocusRequired(this.model);
  }

  dialogDetectionHook() {
    if (this.speakOnDetection && this.model.fieldOfView.isDetected(game.player)) {
      if (canAutoTalk(this.model))
      {
        this.startDialog();
        return true;
      }
      else
        this.model.tasks.addTask("dialogDetectionHook", 1000, 1);
    }
    return false;
  }
}
