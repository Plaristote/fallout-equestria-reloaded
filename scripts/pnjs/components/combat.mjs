import {SkillTargetComponent} from "./skillTarget.mjs";

function evaluatePathfindingOption(target, x, y) {
  return level.getVisionQuality(target.position.x, target.position.y, x, y) - 1;
}

export class CombatComponent extends SkillTargetComponent {
  constructor(model) {
    super(model);
  }

  onTalkTo() {
    if (this.model.isEnemy(level.player))  {
      if (this.model.morale > 0)
        this.displayRandomTextBubble(this.offensiveTextBubbles);
      this.model.requireJoinCombat();
      return false;
    }
    return super.onTalkTo();
  }

  onDamageTaken(amount, dealer) {
    console.log("on damage taken", amount, dealer);
    this.combatTarget = dealer;
  }
  
  findCombatTarget() {
    if (!this.combatTarget || !this.combatTarget.isAlive()) {
      const enemies = this.model.fieldOfView.getEnemies();

      console.log("Detected enemies:", enemies, enemies.length, enemies[0]);
      this.combatTarget = enemies[0];
    }
    return this.combatTarget != null;
  }

  onTurnStart() {
    console.log("on turn start", this.model, this.combatTarget);
    this.findCombatTarget();
    if (!this.combatTarget || !this.combatTarget.isAlive()) {
      const enemies = this.model.fieldOfView.getEnemies();

      console.log("Detected enemies:", enemies, enemies.length, enemies[0]);
      this.combatTarget = enemies[0];
    }
    if (this.combatTarget) {
      const result = this.model.morale > 0 ? this.fightCombatTarget() : this.runAwayFromCombatTarget();

      if (result != null)
        return result;
    }
    level.passTurn(this.model);
  }

  fightCombatTarget() {
    const actions  = this.model.actionQueue;
    const weapon   = this.model.inventory.getEquippedItem("use-1");
    const movement = actions.getReachApCost(this.combatTarget, weapon.getRange(),
      evaluatePathfindingOption.bind(this, this.combatTarget));
    const itemAp   = Math.max(1, actions.getItemUseApCost(this.combatTarget, "use-1"));
    var   ap       = this.model.actionPoints;

    console.log(this.model.statistics.name, "onTurnStart", movement, ap);
    if (movement >= 0) {
      actions.reset();
      if (weapon.maxAmmo > 0 && weapon.ammo === 0) {
        ap -= 2;
        weapon.useMode = "reload";
        actions.pushItemUse(null, "use-1");
      }
      if (movement > 0) {
        ap -= movement;
        actions.pushReach(this.combatTarget, weapon.getRange(),
          evaluatePathfindingOption.bind(this, this.combatTarget));
      }
      while (ap > itemAp) {
        actions.pushItemUse(this.combatTarget, "use-1");
        ap -= itemAp;
      }
      console.log(ap, '/', this.model.actionPoints);
      if (ap != this.model.actionPoints) {
        console.log("Starting action queue");
        return actions.start();
      }
    }
    return null;
  }

  runAwayFromCombatTarget() {
    console.log("runAwayFromTarget", this.combatTarget, this.model.actionPoints);
    if (this.model.actionPoints > 0) {
      this.model.movementMode = "running";
      this.model.moveAway(this.combatTarget);
    }
    if (!this.model.actionQueue.isEmpty())
      return true;
  }

  onActionQueueCompleted() {
    if (level.combat)
       this.onCombatActionQueueCompleted();
  }

  onCombatActionQueueCompleted() {
    console.log("passing turn, action completed");
    if (this.combatTarget && this.model.morale <= 0)
      this.runAwayFromCombatTarget();
    else
      level.passTurn(this.model);
  }
}
