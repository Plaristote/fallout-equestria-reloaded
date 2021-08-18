export class MovementComponent {
  constructor(model) {
    this.model = model;
  }

  followCharacter(character, distance = 3) {
    if (!level.combat) {
      const actions = this.model.getActions();

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
