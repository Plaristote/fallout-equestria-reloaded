import {Shop} from "../shop.mjs";

function overrideBehaviour(object, method, override) {
  const originalMethod = object[method] ? object[method].bind(object) : null;

  object[method] = (...args) => {
    if (override(...args))
      return true;
    return originalMethod ? originalMethod(...args) : false;
  };
}

class MansionShop extends Shop {
  constructor(model) {
    super(model);
    this.model.tasks.addTask("initializeDoorWatch", 100, 1);
  }
  
  initializeDoorWatch() {
    const owner = this.shopOwner;
    const doors = [
      this.model.parent.findObject("backdoor-inside"),
      this.model.parent.findObject("bedroom-door")
    ];

    doors.forEach(door => {
      overrideBehaviour(door.getScriptObject(), "onUse", (user) => {
        if (this.isUnderSurveillance()) {
          level.addTextBubble(owner, "Don't touch that", 2000);
          return true;
        }
        return false;
      });
    });
  }
  
  get guards() {
    return level.findGroup("guards");
  }

  get bed() {
    return this.model.parent.findObject("bedroom.bed");
  }

  openShopRoutine() {
    super.openShopRoutine();
    if (this.isShopOwnerConscious())  {
      this.shopDoors.forEach(door => { door.opened = true; });
      this.shopOwner.getScriptObject().goToWork();
    }
  }

  closeShopRoutine() {
    super.closeShopRoutine();
    if (this.isShopOwnerConscious())
      this.shopOwner.getScriptObject().goToSleep();
  }
}

export function create(model) {
  return new MansionShop(model);
}
