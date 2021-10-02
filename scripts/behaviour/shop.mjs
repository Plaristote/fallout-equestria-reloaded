import {callGuards, AlarmLevel} from "../pnjs/components/alarm.mjs";
import {RoutineComponent} from "./routine.mjs";

const stealReactions = [
  {text: "Hey ! Put that back !",                  time: 2500, color: "yellow"},
  {text: "Don't test me. Give it back.",           time: 2500, color: "orange"},
  {text: "Last warning ! Put that back !",         time: 2500, color: "red"},
  {text: "Guards ! We got a shoplifter over here", time: 3500, color: "red"}
];

const defaultRoutine = [
  { hour: "8",  minute: "30", callback: "openShopRoutine"  },
  { hour: "19", minute: "30", callback: "closeShopRoutine" }
];

export class Shop {
  constructor(model, routine) {
    this.model = model;
    this.maxShopliftAttempts = 3;
    this.routineComponent =  new RoutineComponent(this, routine || defaultRoutine);
  }

  get shopDoors() {
    const doors = [];

    for (var i = 0 ; i < this.model.objects.length ; ++i) {
      if (this.model.objects[i].getObjectType() == "Doorway")
        doors.push(this.model.objects[i]);
    }
    return doors;
  }

  get shopOwner() {
    return this.model.findObject("owner") || this.model.parent.findObject("owner");
  }

  get stealAttemptCount() {
    return this.model.hasVariable("stealAttemptCount")
         ? this.model.getVariable("stealAttemptCount") : 0;
  }
  set stealAttemptCount(value) {
    this.model.setVariable("stealAttemptCount", value);
  }
  
  get opened() {
    return this.routineComponent.getCurrentRoutine().callback === "openShopRoutine";
  }

  openShopRoutine() {
    if (this.shopOwner.isAlive() && !this.shopOwner.unconscious)
      this.shopDoors.forEach(door => door.locked = false);
  }

  closeShopRoutine() {
    if (this.shopOwner.isAlive() && !this.shopOwner.unconscious)
      this.shopDoors.forEach(door => { door.opened = false; door.locked = true });
  }

  isUnderSurveillance() {
    const shopOwner = this.shopOwner;

    if (shopOwner)
    {
      const occupants = this.model.getControlZoneOccupants();

      return occupants.indexOf(shopOwner) >= 0
          && shopOwner.isAlive()
          && !shopOwner.unconscious;
    }
    return false;
  }
  
  onZoneEntered(object) {
    if (!level.combat && this.isUnderSurveillance())
    {
      if (this.opened)
        level.addTextBubble(this.shopOwner, "Welcome !", 2000, "white");
      else
        level.addTextBubble(this.shopOwner, "Hey ! We're closed ! Get out !", 2000, "orange");
    }
  }

  onZoneExited(object) {
    if (!level.combat && this.isUnderSurveillance() && this.opened)
      level.addTextBubble(this.shopOwner, "Come back soon !", 2000, "white");
  }

  onShopliftAttempt(user) {
    if (this.isUnderSurveillance()) {
      var i = this.stealAttemptCount;

      this.stealAttemptCount = i + 1;
      i = Math.min(i, this.maxShopliftAttempts);
      if (i >= this.maxShopliftAttempts)
        callGuards(this.guards, user, AlarmLevel.Arrest);
      level.addTextBubble(this.shopOwner, stealReactions[i].text, stealReactions[i].time, stealReactions[i].color);
      return false;
    }
    return true;
  }
}

export function create(model) {
  return new Shop(model);
}

