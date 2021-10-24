import {QuestHelper} from "./helpers.mjs";

class TutorialQuest extends QuestHelper {
  initialize() {
    console.log("TutorialQuest: on quest start");
    this.model.location = "tutorial";
  }

  getDescription() {
    var html = this.tr("description");

    if (this.lastTrialTriggered())
      html += this.tr("description-with-trial");
    if (this.model.completed)
      html += this.tr("description-completed");
    return html;
  }

  getObjective(name) {
    return {label: this.tr(name), success: this.isObjectiveCompleted(name)};
  }

  getObjectives() {
    const array = [];

    array.push(this.getObjective("exit-cavern"));
    array.push(this.getObjective("leave-first-room"));
    if (this.isObjectiveCompleted("leave-first-room"))
      array.push(this.getObjective("win-first-fight"));
    if (this.lastTrialTriggered())
      array.push(this.getObjective("pass-last-trial"));
    return array;
  }

  completeObjective(objective) {
    if (!this.isObjectiveCompleted(objective)) {
      switch (objective) {
        case "exit-cavern":
          this.model.completed = true;
          game.player.statistics.addExperience(3000);
          game.appendToConsole(this.tr("exit-cavern-success", { xp: 3000 }));
          break ;
        case "leave-first-room":
          game.player.statistics.addExperience(500);
          game.appendToConsole(this.tr("leave-first-room-success", { xp: 500 }));
          break ;
        case "win-first-fight":
          game.player.statistics.addExperience(1500);
          game.appendToConsole(this.tr("win-first-fight-success", { xp: 1500 }));
          break ;
        case "pass-last-trial":
          game.player.statistics.addExperience(1500);
          game.appendToConsole(this.tr("last-trial-success", { xp: 1500 }));
          break ;
      }
    }
    return super.completeObjective(objective);
  }

  lastTrialTriggered() {
    return this.model.hasVariable("lastTrialOn") || this.isObjectiveCompleted("pass-last-trial");
  }

  onCharacterKilled(character, killer) {
    if (character.objectName == "trialDude")
      this.model.completeObjective("pass-last-trial");
  }
}

export function create(model) {
  return new TutorialQuest(model);
}
