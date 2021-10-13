import {CharacterBehaviour} from "./character.mjs";
import {AlarmComponent} from "./components/alarm.mjs";
import {JailComponent} from "../behaviour/jail.mjs";

export class GuardBehaviour extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.alarmComponent = new AlarmComponent(this);
    if (this.textBubbles === undefined) {
      this.textBubbles = [
        { content: i18n.t("bubbles.guard-1"), duration: 2500 },
        { content: i18n.t("bubbles.guard-2"), duration: 2500 },
        { content: i18n.t("bubbles.guard-3"), duration: 2500 },
        { content: i18n.t("bubbles.guard-4"), duration: 3500 }
      ];
    }
  }

  get squad() { return this.model.parent.objects; }

  callSquadToCombat() {
    for (var i = 0 ; i < this.squad.length ; ++i) {
      if (!level.isInCombat(this.squad[i]))
        level.joinCombat(this.squad[i]);
    }
  }

  onCharacterDetected(character) {
    if (this.alarmComponent.onCharacterDetected(character))
      return ;
    super.onCharacterDetected(character);
  }

  onTurnStart() {
    this.callSquadToCombat();
    if (this.alarmComponent.onTurnStart())
      return true;
    return super.onTurnStart();
  }

  onActionQueueCompleted() {
    this.alarmComponent.onActionQueueCompleted();
    super.onActionQueueCompleted();
  }
}

export function create(model) {
  return new GuardBehaviour(model);
}
