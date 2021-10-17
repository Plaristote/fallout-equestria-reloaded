import {QuestHelper} from "./helpers.mjs";

class GemstoneQuest extends QuestHelper {
  initialize() {
  }

  onCharacterKilled(victim, killer) {
    if (victim == level.getObjectByName("trialDog"))
      this.model.failed = true;
  }

  onItemPicked(item) {
    if (item.itemType === "gemstone")
      this.completeObjective("gemstones");
  }

  getObjectives() {
    return [
      {label: this.tr("objective-mine"), success: this.isObjectiveCompleted("gemstones")},
      {label: this.tr("objective-give"), success: this.isObjectiveCompleted("give-gemstones")}
    ];
  }

  onCompleted() {
    const xp = 750;

    game.appendToConsole(i18n.t("messages.quest-complete", {
      title: this.tr("title"),
      xp:    xp
    }));
    game.player.statistics.addExperience(xp);
  }
}

export function create(model) {
  return new GemstoneQuest(model);
}
