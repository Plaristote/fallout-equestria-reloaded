import {CharacterBehaviour} from "../character.mjs";
import {RoutineComponent} from "../../behaviour/routine.mjs";

const routine = [
  { hour: "9",  minute: "30", callback: "goToOffice"  },
  { hour: "12", minute: "30", callback: "goToRestaurant" },
  { hour: "13", minute: "45", callback: "goToOffice" },
  { hour: "22", minute: "30", callback: "goToSleep" }
];

const eatingBubbles = [
  { content: "I'm enjoying my meal. Go away.", duration: 3000, color: "yellow" },
  { content: "If you wanna talk business, see me in my office. In the meantime, scram.", duration: 5000, color: "yellow" }
];

const sleepingBubbles = [
  { content: "Zzzz", duration: 1800, color: "gray" },
  { content: "Hmm.. zzzz...", duration: 2400, color: "gray" }
];

const officeBubbles = [
  { content: "We've got nothing more to talk about.", duration: 3000, color: "yellow" },
  { content: "You should be on your way. Go.", duration: 3000, color: "yellow" },
  { content: "The exit is right behind you. Come on, go away.", duration: 4000, color: "yellow" }
];

class CasinoDirector extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.routineComponent =  new RoutineComponent(this, routine);
  }

  get textBubbles() {
    console.log("Cazino director bubblez ztate:", this.routineComponent.getCurrentRoutine().callback);
    switch (this.routineComponent.getCurrentRoutine().callback) {
      case "goToRestaurant":
        return eatingBubbles;
      case "goToSleep":
        return sleepingBubbles;
    }
    return officeBubbles;
  }

  get bed() {
    return level.findObject("casino.director-room.bed");
  }

  get office() {
    return level.findObject("casino.director-office.chair");
  }

  goToOffice() {
    this.model.actionQueue.pushReach(this.office);
    this.model.actionQueue.start();
  }

  goToRestaurant() {
    const table = level.findObject("casino.restaurant.director-chair");

    this.model.actionQueue.pushReach(table);
    this.model.actionQueue.start();
  }

  goToSleep() {
    this.model.actionQueue.pushReach(this.bed);
    this.model.actionQueue.start();
  }

  onActionQueueCompleted() {
    if (!level.combat)
      this.model.tasks.addTask(this.routineComponent.getCurrentRoutine().callback, 3500, 1);
    super.onActionQueueCompleted();
  }
}

export function create(model) {
  return new CasinoDirector(model);
}