import {CharacterBehaviour} from "../character.mjs";
import {callGuards, AlarmLevel} from "../components/alarm.mjs";

const fearReactions = [
  {text: "Help ! Guards !", time: 2000, color: "yellow"},
  {text: "Aaaah !",         time: 2000, color: "yellow"},
  {text: "Please, help !",  time: 2000, color: "yellow"}
];

export class ShopOwner extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.model.morale = 0;
    this.dialog = "town-trader";
    if (!this.model.hasVariable("workPosition")) {
      this.model.setVariable("workPosition", JSON.stringify({
        x: this.model.position.x, y: this.model.position.y, z: this.model.floor
      }));
    }
  }

  onDamageTaken(amount, dealer) {
    super.onDamageTaken(amount, dealer);
    if (dealer) {
      const reactionIt = Math.floor(Math.random() * 100) % fearReactions.length;
      const reaction = fearReactions[reactionIt];
      callGuards(level.findGroup("guards"), dealer, AlarmLevel.ShootOnSight);
      console.log("Reaction", reactionIt, reaction);
      level.addTextBubble(this.model, reaction.text, reaction.time, reaction.color);
    }
  }

  get shop() {
    const group = this.model.parent.findGroup("shop");
    return group ? group : this.model.parent;
  }

  get bed() {
    return this.model.parent.findObject("bedroom.bed");
  }

  get workPosition() {
    return JSON.parse(this.model.getVariable("workPosition"));
  }

  isAtWork() {
    const t = this.workPosition;
    return this.model.position.x == t.x && this.model.position.y == t.y && this.model.floor == t.z;
  }

  isAtHome() {
    return this.model.parent.findGroup("bedroom").controlZone.isInside(
      this.model.position.x, this.model.position.y, this.model.floor
    );
  }

  goToWork() {
    const t = this.workPosition;
    this.model.actionQueue.pushReachCase(t.x, t.y, t.z, 0);
    this.model.actionQueue.start();
  }
	
  goToSleep() {
    this.model.actionQueue.pushReach(this.bed);
    this.model.actionQueue.start();
  }
  
  onActionQueueCompleted() {
    if (!level.combat) {
      if (this.shop.getScriptObject().opened && !this.isAtWork())
        this.model.tasks.addTask("goToWork", 1000);
      else if (!this.shop.getScriptObject().opened && !this.isAtHome())
        this.model.tasks.addTask("goToSleep", 1000);
    }
    super.onActionQueueCompleted();
  }
}

export function create(model) {
  return new ShopOwner(model);
}
