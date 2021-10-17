import {CharacterBehaviour} from "../character.mjs";

class Cook extends CharacterBehaviour {
  constructor(model) {
    super(model);
  }

  onObservationTriggered() {
    if (!level.combat && this.model.fieldOfView.isDetected(game.player)) {
      const isInZone = game.player.isInZone(level.findGroup("casino.kitchen").controlZone);

      if (isInZone) {
        level.addTextBubble(this.model, i18n.t("bubbles.chase-from-kitchen"), 4000, "yellow");
        level.moveCharacterToZone(level.player, level.findGroup("casino.restaurant").controlZone);
      }
    }
  }
}

export function create(model) {
  return new Cook(model);
}
