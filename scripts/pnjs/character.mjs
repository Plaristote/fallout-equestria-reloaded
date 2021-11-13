import {SceneActorComponent} from "./components/sceneActor.mjs";
import {awarenessHint} from "../cmap/perks/awareness.mjs";

export class CharacterBehaviour extends SceneActorComponent {
  constructor(model) {
    super(model);
    this.xpValue = 25;
    this.canPush = true;
  }

  getHint() {
    if (game.player.statistics.perks.indexOf("awareness") >= 0)
      return awarenessHint(this.model);
    return this.model.statistics.name;
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
    var message = i18n.t("inspection.character", {target: this.model.statistics.name});
    const hpPercentage = this.model.statistics.hpPercentage;
    const gender = this.model.statistics.gender;
    const states = {
      "max":    function (value) { return value >= 100; },
      "fine":   function (value) { return value >= 50; },
      "middle": function (value) { return value >= 25; },
      "low":    function (value) { return value >= 10; },
      "min":    function (value) { return value >= 0; }
    }

    message += ' ';
    if (this.model.isAlive()) {
      for (var key in states) {
        if (states[key](hpPercentage)) {
          message += i18n.t(`inspection.character-state-${gender}`, {state: i18n.t(`inspection.character-states.${key}`)});
          break ;
        }
      }
    }
    else
      message += i18n.t(`inspection.character-dead-${gender}`);
    game.appendToConsole(message);
    return true;
  }

  onPush() {
    this.model.moveAway(game.player);
  }

  onMovementEnded() {
  }

  onDestinationReached() {
    console.log(this.model, "reached destination", this.model.position.x, this.model.position.y);
  }

  onCharacterDetected(character) {
    if (character === level.player && this.dialogDetectionHook())
      return ;
  }
}

export function create(model) {
  return new CharacterBehaviour(model);
}
