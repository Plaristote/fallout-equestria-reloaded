import {QuestHelper} from "./helpers.mjs";

export class Quest extends QuestHelper { 
  getObjectives() {
    const array = [
      {label: this.tr("find-butter"), success: this.isObjectiveCompleted("butter")}
    ];

    if (this.isObjectiveCompleted("butter"))
      array.push({label: this.tr("give-butter"), success: this.isObjectiveCompleted("give")});
    if (this.isObjectiveCompleted("reward"))
      array.push({label: this.tr("got-reward"), success: true });
    return array;
  }

  onItemPicked(item) {
    if (item.itemType === "food-butter")
      this.completeObjective("butter");
  }

  onCompleted() {
    const xp = 1000;

    game.appendToConsole(i18n.t("messages.quest-complete", {
      title: this.tr("title"),
      xp:    xp
    }));
    game.player.statistics.addExperience(xp);
  }
}

export function create(model) {
  return new Quest(model);
}
