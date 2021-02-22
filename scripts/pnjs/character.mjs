export function findPathTo(actions, target) {
  var lowestCost = -1;
  var choice;

  for (var x = -1 ; x <= 1 ; ++x) {
    for (var y = -1 ; y <= 1 ; ++y) {
      const tx = target.x + x;
      const ty = target.y + y;
      const apCost = actions.getMovementApCost(tx, ty);

      //console.log("Trying path", tx, ty, "cost", apCost, "currentLowest", lowestCost);
      if (apCost >= 0 && (lowestCost < 0 || apCost < lowestCost)) {
        lowestCost = apCost;
        choice = { x: tx, y: ty };
        if (apCost == 0)
          break ;
      }
    }
  }
  return { ap: lowestCost, position: choice };
}

export class CharacterBehaviour {
  constructor(model) {
    this.model = model;
    this.textBubbles = [
      { content: "Buck you filly", duration: 5000, color: "red" }
    ];
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
      const enemies = this.model.fieldOfView.getCharacters();

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
