import {WeaponBehaviour} from "./weapon.mjs";

class MouthGun extends WeaponBehaviour {
  constructor(model) {
    super(model);
    this.useModes = ["shoot", "reload"];
    this.skill = "smallGuns";
    this.model.maxAmmo = 6;
    this.fireSound = "gunshot";
  }
  
  get triggersCombat() {
    return this.model.useMode == "shoot";
  }
  
  get requiresTarget() {
    return this.model.useMode == "shoot";
  }
  
  getActionPointCost() {
    if (this.model.useMode == "shoot")
      return 5;
    return 2;
  }
  
  getDamageRange() {
    return [9, 18];
  }
  
  getRange() {
    return 10;
  }

  getAnimationSteps(target) {
    return [
      { type: "Animation", animation: "use", object: this.user },
      { type: "Sprite", name: "effects", animation: "explosion", fromX: this.user.spritePosition.x, fromY: this.user.spritePosition.y, toX: target.spritePosition.x, toY: target.spritePosition.y }
    ]
  }

  onReloaded() {
    const availableAmmo = this.user.inventory.count("9mm-ammo");

    if (availableAmmo > 0) {
      const requiredAmmo = this.model.maxAmmo - this.model.ammo;
      const amount = Math.min(requiredAmmo, availableAmmo);
      
      this.user.inventory.removeItemOfType("9mm-ammo", amount);
      this.model.ammo += amount;
      this.model.useMode = "shoot";
      game.sounds.play("reload");
      return true;
    }
    else {
      game.appendToConsole("Out of ammo !");
      this.user.actionPoints += this.getActionPointCost();
      game.sounds.play("out-of-ammo");
    }
    return false;
  }

  onUnloaded() {
    const amount = this.model.ammo;

    console.log("coucou unloaded", amount);
    if (amount > 0) {
      this.model.ammo = 0;
      this.user.inventory.addItemOfType("9mm-ammo", amount);
      game.sounds.play("reload");
    }
    else
      game.sounds.play("out-of-ammo");
    console.log("bye unlaoded", this.model.ammo);
  }

  triggerUseOn(target) {
    console.log("Trigger uze on trez cher", this.model.useMode);
    if (this.model.useMode == "reload")
      return { steps: [], callback: this.onReloaded.bind(this) };
    else if (this.model.useMode == "unload")
      return { steps: [], callback: this.onUnloaded.bind(this) };
    if (this.model.ammo > 0) {
      this.model.ammo -= 1;
      return super.triggerUseOn(target);
    }
    else {
      game.appendToConsole("Out of ammo !");
      this.user.actionPoints += this.getActionPointCost();
      game.sounds.play("out-of-ammo");
    }
    return false;
  }

  triggerDodgeUse(target) {
    return {
      steps: this.getAnimationSteps(target),
      callback: this.onDodged.bind(this, target)
    };
  }
}

export function create(model) {
  return new MouthGun(model);
}
