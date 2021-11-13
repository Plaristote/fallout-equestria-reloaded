import {CombatComponent} from "./combat.mjs";

export class SceneActorComponent extends CombatComponent {
  onTurnStart() {
    super.onTurnStart();
    if (this.sceneManager && this.sceneManager.active)
      this.sceneManager.onCombatTurn(this.model);
  }

  onDamageTaken(amount, dealer) {
    super.onDamageTaken(amount, dealer);
    if (this.sceneManager && this.sceneManager.active)
      this.sceneManager.onDamageTaken(this.model, dealer);
  }
}