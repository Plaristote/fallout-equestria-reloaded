import {getValueFromRange} from "./random.mjs";

function getEquippedLockpickToolsFor(character) {
  for (var slotI = 1 ; slotI <= 2 ; ++slotI) {
    const item = character.inventory.getEquippedItem(`use-${slotI}`);

    if (item && item.objectName.startsWith("lockpick"))
      return item;
  }
  return null;
}

const racesOpeningDoors = [
  'diamond-dog', 'earth-pony', 'gator-pony', 'griffon', 'pegasus', 'unicorn'
];

export class Door {
  constructor(model) {
    this.model = model;
    this.destructible = true;
    if (!this.model.hasVariable("xp"))
      this.model.setVariable("xp", 25);
  }

  canGoThrough(character) {
    if (racesOpeningDoors.indexOf(character.statistics.race) < 0)
      return false;
    return null;
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
        const xp = this.model.getVariable("xp");

        this.model.locked = !this.model.locked;
        if (user === level.player) {
          game.appendToConsole(this.model.locked
            ? i18n.t("messages.door-locked")
            : i18n.t("messages.door-unlocked")
          );
          if (xp > 0)
            game.appendToConsole(i18n.t("messages.xp-gain", {xp: xp}));
        }
        if (xp > 0) {
          this.model.setVariable("xp", 0);
          user.statistics.addExperience(xp);
        }
      }
      else if (user === level.player) {
        if (roll >= 95 && getValueFromRange(0, 10) > user.statistics.luck) {
          this.model.setVariable("broken", true);
          game.appendToConsole(i18n.t("messages.door-broken"));
        }
        else
          game.appendToConsole(this.model.locked 
            ? i18n.t("messages.door-unlocked-failed")
            : i18n.t("messages.door-locked-failed")
          );
      }
      return success;
    }
    else if (user === level.player)
      game.appendToConsole(i18n.t("messages.lock-is-broken"));
    return false;
  }

  onUseItem(user, item) {
    if (!this.isBroken()) {
      if (item.objectName === this.model.keyName)
      {
        this.model.locked = !this.model.locked;
        if (user === level.player)
          game.appendToConsole(this.model.locked
            ? i18n.t("messages.door-locked")
            : i18n.t("messages.door-unlocked")
          );
      }
      else if (user === level.player)
        game.appendToConsole(i18n.t("messages.nothing-happens"));
    }
    else if (user === level.player)
      game.appendToConsole(i18n.t("messages.door-broken"));
  }
}

export function create(model) {
  return new Door(model);
}
