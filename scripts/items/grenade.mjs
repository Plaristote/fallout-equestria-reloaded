import {WeaponBehaviour} from "./weapon.mjs";
import {explode} from "../behaviour/explosion.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

export class Grenade extends WeaponBehaviour {
  constructor(model) {
    super(model);
    this.zoneTarget = true;
    this.zoneSize = 1;
  }

  attemptToUseAt(x, y) {
    if (this.user.useActionPoints(this.getActionPointCost()))
      return this.triggerUseAt(x, y);
    else
      this.logFailure(i18n.t("messages.not-enough-ap"));
    return false;
  }

  triggerUseAt(x, y) {
    const successRate = this.getUseSuccessRate(x, y);
    const roll = getValueFromRange(0, 100);

    if (roll < 5) {
      x = this.user.position.x;
      y = this.user.position.y;
    }
    else if (roll <= successRate) {
      const fromX = Math.min(x, this.user.position.x);
      const toX   = Math.max(x, this.user.position.x);
      const fromY = Math.min(y, this.user.position.y);
      const toY   = Math.max(y, this.user.position.y);

      x = getValueFromRange(fromX - 2, toX + 2);
      y = getValueFromRange(fromY - 2, toY + 2);
    }
    return {
      steps:    this.getAnimationSteps(x, y),
      callback: this.useAt.bind(this, x, y)
    };
  }

  useAt(x, y) {
    const damage = getValueFromRange(...this.getDamageRange());

    explode(
      { x: x, y: y },
      this.zoneSize,
      damage,
      null,
      this.user
    );
    this.model.remove(1);
    return true;
  }

  getAnimationSteps(x, y) {
    const steps  = this.getUseAnimation();
    const from   = this.user.spritePosition;
    const target = level.getRenderPositionForTile(x, y);

    if (from.x != to.x || from.y != to.y) {
      steps.push({
        type: "Sprite",
        name: "effects",
        animation: "grenade-throw",
        fromX: from.x, fromY: from.y,
        toX: target.x, toY: target.y
      });
    }
    return steps;
  }

  getUseSuccessRate(x, y) {
    const attackerWeaponSkill = this.getUserSkillValue();
    const distance = this.user.getDistance(x, y);
    var baseToHit = attackerWeaponSkill;

    baseToHit -= 25;
    baseToHit += 8 * Math.max(0, this.getUserPerception() - 2);
    baseToHit -= Math.max(0, distance - 1) * 7;
    return Math.max(0, Math.min(baseToHit, 95));
  }

  getRange() {
    const strength = this.user.statistics.strength;

    return Math.max(3, strength + 1);
  }

  getDamageRange() {
    return [15, 25];
  }
}

export function create(model) {
  return new Grenade(model);
}
