import {CharacterBehaviour} from "./character.mjs";
import {getRandomCaseInZone} from "../behaviour/pathfinding.mjs";

export const textBubbles = [
  {content: i18n.t("bubbles.passerby-1"), duration: 5000 },
  {content: i18n.t("bubbles.passerby-2"), duration: 5000 },
  {content: i18n.t("bubbles.passerby-3"), duration: 5000 }
];

export class PasserbyBehaviour extends CharacterBehaviour {
  constructor(model, locations) {
    super(model);
    this.passerbyLocations = locations;
    this.scheduleNextTravel();
    this.textBubbles = textBubbles;
  }

  goToNextLocation() {
    var travelStarted = false;    
    
    if (!level.combat) {
      const locationIt = Math.floor(Math.random() * 100) % this.passerbyLocations.length;
      const location = this.passerbyLocations[locationIt];
      var position = {x: 0, y: 0 };
      
      if (typeof location === "string")
        position = getRandomCaseInZone(level.findGroup(location).controlZone);
      else
        position = location;
      this.model.actionQueue.pushReachNear(position.x, position.y, 3);
      travelStarted = this.model.actionQueue.start();
    }
    if (!travelStarted)
      this.scheduleNextTravel();
  }
  
  scheduleNextTravel() {
    const interval = Math.ceil(Math.random() * 15) + 10;
  
    if (this.model.tasks.hasTask("goToNextLocation"))
      this.model.tasks.removeTask("goToNextLocation");
    console.log("Next trigger in", interval);
    this.model.tasks.addTask("goToNextLocation", interval * 1000, 1);
  }
  
  onActionQueueCompleted() {
    super.onActionQueueCompleted();
    this.scheduleNextTravel();
  }
}
