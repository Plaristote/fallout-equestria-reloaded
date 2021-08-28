import {CharacterBehaviour} from "./character.mjs";

const dialogLines = {
  yell: "Hey ! You got here, at last !",
  step1: [
    { content: "There's a door over there. It's locked, but it's the safest way to go.", duration: 5000 },
    { content: "I wouldn't recommend taking the western tunnel.", duration: 3000 },
    { content: "Are you gonna find us a way out or what ?", duration: 4000 },
    { content: "I wish I had something witty to say...", duration: 3500 },
    { content: "Sorry my dialogues options are limited. The game maker lacks creativity.", duration: 4000 }
  ]
};

class TutoChar extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.dialog = "tutoCharIntro";
    if (!model.hasVariable("startRoutine"))
      this.startRoutine();
  }

  get textBubbles() {
    if (this.model.getVariable("startRoutine") == 2)
      return dialogLines.step1;
    return [];
  }

  startRoutine() {
    console.log("TutoChar startRoutine called");
    this.model.setVariable("startRoutine", 1);
    this.model.tasks.addTask("startRoutineTask", 100, 1);
    this.model.tasks.addTask("startRoutineYell", 1000, 1);
  }

  startRoutineTask() {
    console.log("Follow player triggered");
    this.followPlayer();
  }

  startRoutineYell() {
    level.addTextBubble(this.model, dialogLines.yell, 3000, "yellow");
  }

  joinedPlayer() {
    game.getPlayerParty().addCharacter(this.model);
    this.model.statistics.faction = "player";
    this.model.tasks.addTask("followPlayer", 8000, 0);
  }

  followPlayer() {
    if (!level.combat) {
      const actions = this.model.actionQueue;

      this.model.movementMode = "running";
      actions.reset();
      actions.pushReach(level.player, 3);
      actions.start();
    }
  }

  onActionQueueCompleted() {
    if (!level.combat && this.model.getVariable("startRoutine") === 1) {
      this.model.setVariable("startRoutine", 2);
      if (level.player.getDistance(this.model) > 3)
        this.followPlayer();
      else {
        level.initializeDialog(this.model);
        delete this.dialog;
      }
    }
    else
      super.onActionQueueCompleted();
  }
}

export function create(model) {
  console.log("CREATING TUTO CHAR");
  return new TutoChar(model);
}

