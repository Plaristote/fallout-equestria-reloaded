import {WeaponBehaviour} from "./weapon.mjs";
import {explode} from "../behaviour/explosion.mjs";
import {getValueFromRange, randomCheck} from "../behaviour/random.mjs";

export class Grenade extends WeaponBehaviour {
  constructor(model) {
    super(model);
    this.zoneTarget = true;
    this.zoneSize = 1;
    this.useModes = ["throw"];
  }

  attemptToUseAt(x, y) {
    if (this.user.useActionPoints(this.getActionPointCost()))
      return this.triggerUseAt(x, y);
    else
      this.logFailure(i18n.t("messages.not-enough-ap"));
    return false;
  }

  disperseThrow(x, y) {
    const fromX = Math.min(x, this.user.position.x);
    const toX   = Math.max(x, this.user.position.x);
    const fromY = Math.min(y, this.user.position.y);
    const toY   = Math.max(y, this.user.position.y);
    const distanceX = toX - fromX;
    const distanceY = toY - fromY;

    x = x + getValueFromRange(-1, distanceX) * (toX < fromX ? 1 : -1);
    y = y + getValueFromRange(-1, distanceY) * (toY < fromY ? 1 : -1);
    return [x,y];
  }

  triggerUseAt(x, y) {
    const successRate = this.getUseAtSuccessRate(x, y);
    const roll = getValueFromRange(0, 100);

    randomCheck(successRate, {
      criticalFailure: () => {
        game.appendToConsole(i18n.t("messages.weapons.critical-failure", {
          user: this.user.statistics.name,
          item: this.trName
	}));
        x = this.user.position.x;
        y = this.user.position.y;
      },
      failure: () => {
        const result = this.disperseThrow(x, y);

        x = result[0];  y = result[1];
      },
      success: () => {}
    });
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

    if (from.x != target.x || from.y != target.y) {
      steps.push({
        type: "Sprite",
        name: "effects",
        animation: "grenade-throw",
        fromX: from.x, fromY: from.y,
        toX: target.x, toY: target.y,
        speed: 250
      });
    }
    return steps;
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
