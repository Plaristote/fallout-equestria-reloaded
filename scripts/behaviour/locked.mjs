import {getValueFromRange} from "./random.mjs";

function getEquippedLockpickToolsFor(character) {
  for (var slotI = 1 ; slotI <= 2 ; ++slotI) {
    const item = character.inventory.getEquippedItem(`use-${slotI}`);

    if (item && item.objectName.startsWith("lockpick"))
      return item;
  }
  return null;
}

export class LockedComponent {
  constructor(parent, options = {}) {
    this.lockpickLevel = options.lockpickLevel || 1;
    this.breakable = options.breakable || true;
    this.model = parent.model;
    this.onSuccess = options.onSuccess;
    this.onFailure = options.onFailure;
    this.onCriticalFailure = options.onCriticalFailure;
    if (!this.model.hasVariable("xp"))
      this.model.setVariable("xp", options.xp || 25);
  }
  
  toggleLocked() {
    this.model.locked = !this.model.locked;
  }
  
  isBroken() {
    return this.model.hasVariable("broken") ? this.model.getVariable("broken") : false;
  }

  onUseLockpick(user) {
    if (!this.isBroken()) {
      const difficultyMalus = this.lockpickLevel * 25;
      const item            = getEquippedLockpickToolsFor(user);
      const lockpick        = user.statistics.lockpick + (item ? item.scriptCall("getLockpickBonus") : 0);
      const roll            = getValueFromRange(0, 100);
      const success         = lockpick - difficultyMalus >= roll;

      if (success) {
        const xp = this.model.getVariable("xp");

        this.toggleLocked();
        if (user === level.player) {
          this.onSuccess(user);
          if (xp > 0)
            game.appendToConsole(i18n.t("messages.xp-gain", {xp: xp}));
        }
        if (xp > 0) {
          this.model.setVariable("xp", 0);
          user.statistics.addExperience(xp);
        }
      }
      else if (user === level.player) {
        if (this.breakable && roll >= 95 && getValueFromRange(0, 10) > user.statistics.luck) {
          this.model.setVariable("broken", true);
          this.onCriticalFailure(user);
        }
        else
          this.onFailure(user);
      }
      return success;
    }
    else if (user === level.player)
      game.appendToConsole(i18n.t("messages.lock-is-broken"));
    return false;
  }
}
