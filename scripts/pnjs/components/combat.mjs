import {SkillTargetComponent} from "./skillTarget.mjs";

export class CombatComponent extends SkillTargetComponent {
  constructor(model) {
    super(model);
  }

  onDamageTaken(amount, dealer) {
    console.log("on damage taken", amount, dealer);
    this.combatTarget = dealer;
  }

  onTurnStart() {
    console.log("on turn start", this.model, this.combatTarget);
    if (!this.combatTarget || !this.combatTarget.isAlive()) {
      const enemies = this.model.fieldOfView.getEnemies();

      console.log("Detected enemies:", enemies, enemies.length, enemies[0]);
      this.combatTarget = enemies[0];
    }
    if (this.combatTarget) {
      const actions  = this.model.actionQueue;
      const weapon   = this.model.inventory.getEquippedItem("use-1");
      const movement = actions.getReachApCost(this.combatTarget, weapon.getRange());
      const itemAp   = Math.max(1, actions.getItemUseApCost(this.combatTarget, "use-1"));
      var   ap       = this.model.actionPoints;

      console.log(this.model.statistics.name, "onTurnStart", movement, ap);
      if (movement >= 0) {
        actions.reset();
        if (movement > 0) {
          ap -= movement;
          actions.pushReach(this.combatTarget, weapon.getRange());
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
    }
    level.passTurn(this.model);
  }

  onActionQueueCompleted() {
    console.log("action queue completed", level.combat);
    if (level.combat)
       this.onCombatActionQueueCompleted();
  }

  onCombatActionQueueCompleted() {
    console.log("passing turn, action completed");
    level.passTurn(this.model);
  }
}
