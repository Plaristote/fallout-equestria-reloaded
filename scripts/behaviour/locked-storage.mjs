import {LockedComponent} from "./locked.mjs";

class LockedStorage {
  constructor(model) {
    this.model = model;
    this.lockedComponent = new LockedComponent(this, {
      xp: 25,
      lockpickLevel: 2,
      breakable: true,
      onSuccess: this.onLockpickSuccess.bind(this),
      onFailure: this.onLockpickFailure.bind(this),
      onCriticalFailure: this.onLockpickCriticalFailure.bind(this)
    });
    this.lockedComponent.toggleLocked = this.toggleLocked.bind(this);
  }
  
  get locked() {
    return this.model.hasVariable("locked") ? this.model.getVariable("locked") : true
  }
  
  toggleLocked() {
    this.model.setVariable("locked", !this.locked);
  }
  
  onLockpickSuccess(user) {
    const word = (this.locked ? '' : 'un') + 'locked';
    game.appendToConsole(i18n.t(`messages.locker-${word}`));
  }
  
  onLockpickFailure(user) {
    const word = (this.locked ? 'un' : '') + 'locked';
    game.appendToConsole(i18n.t(`messages.locker-${word}-failed`));
  }
  
  onLockpickCriticalFailure(user) {
    game.appendToConsole(i18n.t("messages.locker-broken"));
  }

  onUse() {
    if (this.locked) {
      game.appendToConsole(i18n.t("messages.locker-locked"));
      return true;
    }
    return false;
  }
  
  onUseLockpick(user) {
    return this.lockedComponent.onUseLockpick(user);
  }
  
  onUseSteal() {
    return !this.locked;
  }
}

export function create(model) {
  return new LockedStorage(model);
}
