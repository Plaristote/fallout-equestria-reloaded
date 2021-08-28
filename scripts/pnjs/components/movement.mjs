export class MovementComponent {
  constructor(model) {
    this.model = model;
  }

  onMovementStart() {
    if (this.model.movementMode === "running")
      this.model.toggleSneaking(false);
  }

  followCharacter(character, distance = 3) {
    if (!level.combat) {
      const actions = this.model.actionQueue;

      this.model.movementMode = "running";
      actions.reset();
      actions.pushReach(level.player, distance);
      actions.start();
    }
  }

  followPlayer(distance = 3) {
    this.followCharacter(level.player, distance);
  }
}
