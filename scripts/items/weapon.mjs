import {ItemBehaviour} from "./item.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

export class WeaponBehaviour extends ItemBehaviour {
  constructor(model) {
    super(model);
    if (this.triggersCombat == undefined)
      this.triggersCombat = true;
    if (this.skill == undefined)
      this.skill = "unarmed";
    if (this.hitSound == undefined)
      this.hitSound = "hoof2hoof/punch";
  }

  isValidTarget(object) {
    return object.getObjectType() === "Character" && object !== this.user && object.isAlive();
  }

  isRangedWeapon() {
    return this.skill != "unarmed" && this.skill != "meleeWeapons";
  }

  isInRange(target) {
    if (this.isRangedWeapon() && this.user)
      return this.user.getDistance(target) < this.getRange();
    return super.isInRange(target);
  }

  getUserSkillValue() {
    return this.user.statistics[this.skill];
  }

  getUserPerception() {
   return this.user.statistics.perception;
  }

  getTargetArmorClass(target) {
    return target.statistics.armorClass;
  }

  getRange() {
    return 1.6;
  }

  triggerUseOn(target) {
    const successRate = this.getUseSuccessRate(target);
    const roll        = getValueFromRange(0, 100);

    console.log("ROLL", roll, '/', successRate);
    if (this.fireSound)
      level.sounds.play(this.fireSound);
    if (roll > successRate)
      return this.triggerDodgeUse(target);
    if (roll < 5)
      return this.triggerCriticalFailure(target);
    return super.triggerUseOn(target);
  }

  triggerDodgeUse(target) {
    return {
      steps: this.getUseAnimation(),
      callback: this.onDodged.bind(this, target)
    }
  }

  triggerCriticalFailure(target) {
    return this.triggerDodgeUse(target);
  }

  onDodged(target) {
    target.attackedBy(this.user);
    game.appendToConsole(
      i18n.t("messages.weapons.dodge", {
        user: this.user.statistics.name,
        target: target.statistics.name
      })
    );
    level.sounds.play("dodge");
    return true;
  }

  useOn(target) {
    var damage = getValueFromRange(...this.getDamageRange());

    damage -= target.statistics.damageResistance;
    damage = Math.max(0, damage);
    game.appendToConsole(
      i18n.t("messages.weapons.use", {
        user: this.user.statistics.name,
        item: this.model.objectName,
        target: target.statistics.name,
        damage: damage
      })
    );
    target.takeDamage(damage, this.user);
    level.sounds.play(this.hitSound);
    return true;
  }

  getUseSuccessRate(target) {
    const attackerWeaponSkill = this.getUserSkillValue();
    const defenderArmorClass  = this.getTargetArmorClass(target);
    var   baseToHit           = attackerWeaponSkill - defenderArmorClass;

    if (this.isRangedWeapon())
    {
      const distance = this.user.getDistance(target);

      baseToHit -= 25;
      baseToHit += 8 * Math.max(0, this.getUserPerception() - 2);
      baseToHit -= Math.max(0, distance - 1) * 7;
    }
    console.log(attackerWeaponSkill, defenderArmorClass);
    return Math.max(0, Math.min(baseToHit, 95));
  }
}

export const Weapon = WeaponBehaviour;
