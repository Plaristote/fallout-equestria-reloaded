import {Door} from "./door.mjs";

export class TutoDoor extends Door {
  constructor(model) {
    super(model);
    this.lockedComponent.breakable = false;
  }
  
  onUse() {
    if (this.model.locked && !this.model.hasVariable("tutoOver")) {
      level.tutorial.page = 3;
      level.tutorial.visible = true;
    }
    return false;
  }
  
  onUseLockpick(user) {
    const result = super.onUseLockpick(user);

    if (!this.model.hasVariable("tutoOver")) {
      if (this.isBroken())
        level.addTextBubble(level.player, "Oh. You broke the lock. There are always several ways to achieve an objective. Look around for something that might help you break the door down", 10000);
      else if (this.model.locked)
        level.addTextBubble(level.player, "If you do not succeed on the first try, you can always try again. Unless the lock breaks. Then you'll have to look for another way to open the door", 10000);
      else {
        this.model.setVariable("tutoOver", 1);
        level.addTextBubble(level.player, "Well done, you picked the lock. You can now open the door.", 3000);
      }
    }
    return result;
  }
}

export function create(model) {
  return new TutoDoor(model);
}
