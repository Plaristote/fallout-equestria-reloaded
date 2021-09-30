const stealReactions = [
  {text: "Hey ! Put that back !",                  time: 2500, color: "yellow"},
  {text: "Don't test me. Give it back.",           time: 2500, color: "orange"},
  {text: "Last warning ! Put that back !",         time: 2500, color: "red"},
  {text: "Guards ! We got a shoplifter over here", time: 3500, color: "red"}
];

export class Shop {
  constructor(model) {
    this.model = model;
  }
  
  get shopOwner() {
    return this.model.findObject("owner");
  }

  get stealAttemptCount() {
    return this.model.hasVariable("stealAttemptCount")
         ? this.model.getVariable("stealAttemptCount") : 0;
  }
  set stealAttemptCount(value) {
    this.model.setVariable("stealAttemptCount", value);
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
      level.addTextBubble(this.shopOwner, "Welcome !", 2000, "white");
  }
  
  onZoneExited(object) {
    if (!level.combat && this.isUnderSurveillance())
      level.addTextBubble(this.shopOwner, "Come back soon !", 2000, "white");
  }

  onShopliftAttempt(user) {
    if (this.isUnderSurveillance()) {
      var i = this.stealAttemptCount;

      this.stealAttemptCount = i + 1;
      i = Math.min(i, 3);
      if (i >= 0)
      {
        //this.model.setAsEnemy(user);
        this.callGuards(user);
      }
      level.addTextBubble(this.shopOwner, stealReactions[i].text, stealReactions[i].time, stealReactions[i].color);
      return false;
    }
    return true;
  }

  callGuards(target) {
    console.log("Calling guards", this.guards, this.guards.objects.length,
      "to position", target.position.x, target.position.y, target);
    for (var i = 0 ; i < this.guards.objects.length ; ++i) {
      this.guards.objects[i].getScriptObject().receiveAlarmSignal(
        target.position.x, target.position.y, target
      );
    }
  }
}

export function create(model) {
  return new Shop(model);
}

