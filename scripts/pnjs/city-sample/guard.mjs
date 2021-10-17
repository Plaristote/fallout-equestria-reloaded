import {GuardBehaviour} from "../guard.mjs";
import {GuardComponent} from "../components/guard.mjs";
import {PatrolComponent, PatrolMode} from "../components/patrol.mjs";
import {JailComponent} from "../../behaviour/jail.mjs";

const patrolSpots = [
  {x: 28, y: 58},
  {x: 62, y: 58},
  {x: 62, y: 38},
  {x: 28, y: 58}
];

export class Guard extends GuardBehaviour {
  constructor(model) {
    super(model);
    this.jailComponent  = new JailComponent(this, "police-station.jail");
    if (this.model.parent.path == "guards")
      this.initializePatrolMode();
    else
      this.initializeGuardMode();
  }

  initializePatrolMode() {
    this.patrolComponent             = new PatrolComponent(this);
    this.patrolComponent.patrolSpots = patrolSpots;
    this.patrolComponent.mode        = PatrolMode.Random;
  }

  initializeGuardMode() {
    this.guardComponent = new GuardComponent(this);
  }

  onCharacterDetected(character) {
    if (this.jailComponent.onCharacterDetected(character))
      return ;
    super.onCharacterDetected(character);
  }
}

export function create(model) {
  return new Guard(model);
}
