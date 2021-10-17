import {CharacterBehaviour} from "./character.mjs";

class TutorialDude extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.dialog = "myDialog";
  }

  get speakOnDetection() {
    return !this.model.hasVariable("talkedTo");
  }

  onTalkTo() {
    game.quests.getQuest("tutorial").setVariable("lastTrialOn", 1);
    return super.onTalkTo();
  }

  onDamageTaken(amount, dealer) {
    console.log("on tutorial dude took damage", this.model.statistics.hitPoints);
    if (this.model.statistics.hitPoints <= 10 && this.model.statistics.hitPoints > 0)
    {
      level.leaveCombat(this.model);
      level.tryToEndCombat();
      this.model.setVariable("surrender", 1);
      this.model.setAsFriendly(game.player);
      this.model.setVariable("entryPoint", 2);
      level.initializeDialog(this.model);
      game.quests.getQuest("tutorial").completeObjective("pass-last-trial");
    }
    else
      super.onDamageTaken(amount, dealer);
  }
}

export function create(model) {
  return new TutorialDude(model);
}
