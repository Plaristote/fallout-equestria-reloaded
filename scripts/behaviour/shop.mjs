import {callGuards, AlarmLevel} from "../pnjs/components/alarm.mjs";
import {RoutineComponent} from "./routine.mjs";

const stealReactions = [
  {content: i18n.t("bubbles.steal-warning-1"), duration: 2500, color: "yellow"},
  {content: i18n.t("bubbles.steal-warning-2"), duration: 2500, color: "orange"},
  {content: i18n.t("bubbles.steal-warning-3"), duration: 4000, color: "red"},
  {content: i18n.t("bubbles.steal-warning-4"), duration: 3500, color: "red"}
];

const entryReactions = [
  {content: i18n.t("bubbles.shop-welcome-1"), duration: 2500},
  {content: i18n.t("bubbles.shop-welcome-2"), duration: 2500},
  {content: i18n.t("bubbles.shop-welcome-3"), duration: 2500}
];

const exitReactions = [
  {content: i18n.t("bubbles.shop-goodbye-1"), duration: 2500},
  {content: i18n.t("bubbles.shop-goodbye-2"), duration: 2500}
];

const defaultRoutine = [
  { hour: "8",  minute: "30", callback: "openShopRoutine"  },
  { hour: "19", minute: "30", callback: "closeShopRoutine" }
];
        
function displayRandomTextBubble(character, options) {
  character.getScriptObject().displayRandomTextBubble(options);
}

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

  isShopOwnerConscious() {
    return this.shopOwner.isAlive() && !this.shopOwner.unconscious;
  }

  openShopRoutine() {
    if (this.isShopOwnerConscious())
      this.shopDoors.forEach(door => door.locked = false);
  }

  closeShopRoutine() {
    if (this.isShopOwnerConscious()) {
      this.shopDoors.forEach(door => { door.opened = false; door.locked = true });
      this.chaseCustomers();
    }
  }
  
  chaseCustomers() {
    const occupants = this.model.getControlZoneOccupants();
    
    for (var i = 0 ; i < occupants.length ; ++i) {
      if (occupants[i].getObjectType() == "Character" && this.shopOwner.fieldOfView.isDetected(occupants[i])) {
        level.moveCharacterToZone(occupants[i], "");
        if (occupants[i] === level.player)
          level.cameraFocusRequired(level.player);
      }
    }
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
        displayRandomTextBubble(this.shopOwner, entryReactions);
      else
        level.addTextBubble(this.shopOwner, i18n.t("bubbles.shop-closed"), 2000, "orange");
    }
  }

  onZoneExited(object) {
    if (!level.combat && this.isUnderSurveillance() && this.opened)
      displayRandomTextBubble(this.shopOwner, exitReactions);
  }

  onShopliftAttempt(user) {
    if (this.isUnderSurveillance()) {
      var i = this.stealAttemptCount;

      this.stealAttemptCount = i + 1;
      i = Math.min(i, this.maxShopliftAttempts);
      if (i >= this.maxShopliftAttempts)
        callGuards(this.guards, user, AlarmLevel.Arrest);
      level.addTextBubble(this.shopOwner, stealReactions[i].content, stealReactions[i].duration, stealReactions[i].color);
      return false;
    }
    return true;
  }
}

export function create(model) {
  return new Shop(model);
}

