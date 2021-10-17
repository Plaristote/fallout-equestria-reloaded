import {CharacterBehaviour} from "../character.mjs";

const bubbles = [
  {content: "Watch yourself in front of the boss.", duration: 4500, color: "yellow"},
  {content: "Boss is tough, but fair.", duration: 3000, color: "yellow"},
  {content: "Don't fool around. I'm watching you.", duration: 4500, color: "yellow"}
];

class CasinoBodyguard extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.textBubbles = bubbles;
    if (!this.model.tasks.hasTask("followTheBoss"))
      this.model.tasks.addTask("followTheBoss", 1000, 0);
  }

  get boss() {
    return level.findObject("casino.director");
  }

  followTheBoss() {
    if (this.model.actionQueue.isEmpty()) {
      const boss = this.boss;
      const currentRoutine = boss.getScriptObject().routineComponent.getCurrentRoutine().callback;

      if (currentRoutine !== "goToSleep") {
        const office = level.findGroup("casino.director-office");

        console.log("-> isInZone", boss.isInZone(office.controlZone));
        if (boss.isInZone(office.controlZone))
          this.goToOfficeStation();
        else if (this.model.getDistance(boss) > 2) {
          this.model.actionQueue.pushReach(boss, 2);
          this.model.actionQueue.start();
        }
      }
      else
        this.goToOfficeStation();
    }
  }

  goToOfficeStation() {
    if (this.model.position.x !== 25 || this.model.position.y !== 5 || this.model.floor !== 2) {
      this.model.actionQueue.pushReachCase(25, 5, 0);
      this.model.actionQueue.start();
    }
    else
      this.model.lookTo(32, 12);
  }
}

export function create(model) {
  return new CasinoBodyguard(model);
}
