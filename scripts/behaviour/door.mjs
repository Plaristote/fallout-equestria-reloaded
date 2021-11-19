import {getValueFromRange} from "./random.mjs";
import {LockedComponent} from "./locked.mjs";

const racesOpeningDoors = [
  'diamond-dog', 'earth-pony', 'gator-pony', 'griffon', 'pegasus', 'unicorn'
];

export class Door {
  constructor(model) {
    this.model = model;
    this.destructible = true;
    this.lockedComponent = new LockedComponent(this, {
      xp: 25,
      lockpickLevel: this.model.lockpickLevel,
      breakable: true,
      onSuccess: this.onLockpickSuccess.bind(this),
      onFailure: this.onLockpickFailure.bind(this),
      onCriticalFailure: this.onLockpickCriticalFailure.bind(this)
    });
  }
  
  onLockpickSuccess(user) {
    game.appendToConsole(this.model.locked
      ? i18n.t("messages.door-locked")
      : i18n.t("messages.door-unlocked")
    );
  }
  
  onLockpickFailure(user) {
    game.appendToConsole(this.model.locked 
      ? i18n.t("messages.door-unlocked-failed")
      : i18n.t("messages.door-locked-failed")
    );
  }
  
  onLockpickCriticalFailure(user) {
    game.appendToConsole(i18n.t("messages.door-broken"));
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
    return this.lockedComponent.onUseLockpick(user);
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
