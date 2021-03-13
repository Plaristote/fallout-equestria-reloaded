import {getValueFromRange} from "../behaviour/random.mjs";
import {findPathTo} from "../behaviour/pathfinding.mjs";

export class CharacterBehaviour {
  constructor(model) {
    this.model = model;
    this.xpValue = 25;
    this.textBubbles = [
      { content: "Lorem ipsum dolor sit amet, et toute ces sortes de choses.", duration: 5000, color: "yellow" }
    ];
    if (!this.model.hasVariable("metabolismEnabled"))
    {
      this.model.setVariable("metabolismEnabled", true);
      this.model.tasks.addTask("updateMetabolism", 86400000, 0);
    }
  }

  updateMetabolism() {
    if (this.model.isAlive()) {
      const maxHitPoints = this.model.statistics.maxHitPoints;
      const healingRate  = this.model.statistics.healingRate;
      const currentHp    = this.model.statistics.hitPoints;
      
      this.model.statistics.hitPoints = Math.min(currentHp + healingRate, maxHitPoints);
    }
  }

  getAvailableInteractions() {
    const interactions = ["look", "use-skill"];

    if (this.dialog || this.textBubbles)
      interactions.unshift("talk-to");
  
    return interactions;
  }

  onLook() {
    var message = "You see " + this.model.statistics.name + ".";

    message += " " + this.model.statistics.hitPoints + "/" + this.model.statistics.maxHitPoints + " HP";
    game.appendToConsole(message);
    return true;
  }

  onTalkTo() {
    if (this.dialog)
      level.initializeDialog(this.model, this.dialog);
    else
      this.displayRandomTextBubble();
  }

  displayRandomTextBubble() {
    const it = Math.floor(Math.random() * this.textBubbles.length);
    const textBubble = this.textBubbles[it];

    level.addTextBubble(this.model, textBubble.content, textBubble.duration, textBubble.color || "white");
  }

  onUseMedicine(user) {
    if (!level.isInCombat(user))
    {
      const stats   = this.model.statistics;
      const maxHeal = stats.maxHitPoints - stats.hitPoints;
      var   healed  = getValueFromRange(0, 10) + user.statistics.medicine / 10;
 
      healed = healed * user.statistics.medicine / 100;
      healed = Math.min(healed, maxHeal);
      stats.hitPoints += healed;
      game.appendToConsole(`${user.statistics.name} used medicine on ${stats.name} and healed it for ${healed} hit points.`);
      level.addTextBubble(this.model, "Thank you kindly !", 5000, "lightgreen");
      return true;
    }
    else
      game.appendToConsole("You can't use medicine during combat.");
    return false;
  }

  onUseSteal(user) {
  }

  onMovementEnded() {
  }

  onDestinationReached() {
    console.log(this.model, "reached destination", this.model.position.x, this.model.position.y);
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
        if (ap != this.model.actionPoints)
          return actions.start();
      }
    }
    level.passTurn(this.model);
  }

  onActionQueueCompleted() {
    if (level.combat)
       this.onCombatActionQueueCompleted();
  }

  onCombatActionQueueCompleted() {
	  console.log("passing turn, action completed");
    level.passTurn(this.model);
  }
}

export function create(model) {
  return new CharacterBehaviour(model);
}
