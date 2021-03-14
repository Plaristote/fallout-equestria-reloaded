import {getValueFromRange} from "./random.mjs";

function getEquippedLockpickToolsFor(character) {
  for (var slotI = 1 ; slotI <= 2 ; ++slotI) {
    const item = character.inventory.getEquippedItem(`use-${slotI}`);

    if (item && item.objectName.startsWith("lockpick"))
      return item;
  }
  return null;
}

export class Door {
  constructor(model) {
    this.model = model;
  }

  isBroken() {
    return this.model.hasVariable("broken") && this.model.getVariable("broken") == true;
  }

  onUseLockpick(user) {
    if (!this.isBroken()) {
      const difficultyMalus = this.model.lockpickLevel * 25;
      const item            = getEquippedLockpickToolsFor(user);
      const lockpick        = user.statistics.lockpick + (item ? item.scriptCall("getLockpickBonus") : 0);
      const roll            = getValueFromRange(0, 100);
      const success         = lockpick - difficultyMalus >= roll;

      if (success) {
        this.model.locked = !this.model.locked;
        if (user === level.player)
          game.appendToConsole(this.model.locked ? "You successfully locked the door" : "You successfully unlocked the door");
      }
      else if (user === level.player) {
        if (roll >= 95 && getValueFromRange(0, 10) > user.statistics.luck) {
          this.model.setVariable("broken", true);
          game.appendToConsole("You critically failed and broke the lock");
        }
        else
          game.appendToConsole(this.model.locked ? "You failed to unlock the door" : "You failed to lock the door");
      }
      return success;
    }
    else if (user === level.player)
      game.appendToConsole("The lock is broken");
    return false;
  }

  onUseItem(user, item) {
    if (!this.isBroken()) {
      if (item.objectName === this.model.keyName)
      {
        this.model.locked = !this.model.locked;
        if (user === level.player)
          game.appendToConsole(this.model.locked ? "You successfully locked the door" : "You successfully unlocked the door");
      }
      else if (user === level.player)
        game.appendToConsole("That does nothing.");
    }
    else if (user === level.player)
      game.appendToConsole("The lock is broken");
  }
}

export function create(model) {
  return new Door(model);
}
