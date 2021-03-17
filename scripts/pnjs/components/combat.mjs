import {SkillTargetComponent} from "./skillTarget.mjs";
import {findPathTo} from "../../behaviour/pathfinding.mjs";

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
      const actions = this.model.getActions();
      const movement = findPathTo(actions, this.combatTarget.getPosition());
      const itemAp = Math.max(1, actions.getItemUseApCost(this.combatTarget, "use-1"));
      var   ap = this.model.actionPoints;

      console.log(this.model.statistics.name, "onTurnStart", movement.ap, ap);
      if (movement.ap >= 0) {
        actions.reset();
        if (movement.ap > 0) {
          ap -= movement.ap;
          actions.pushMovement(movement.position.x, movement.position.y);
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
