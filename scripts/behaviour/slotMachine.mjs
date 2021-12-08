class SlotMachine {
  constructor(model) {
    this.model = model;
  }

  getAvailableInteractions() {
    return ["use", "look", "use-skill"];
  }

  onUse(user) {
    if (user === game.player) {
      const currentUser = level.getOccupantAt(this.model.position.x, this.model.position.y + 1, this.model.currentFloor);

      if (currentUser && currentUser !== game.player)
        game.appendToConsole(i18n.t("messages.slot-machine-in-use"));
      else
        level.initializeDialog(this.model, "slotMachine");
    }
  }
}

export function create(model) {
  return new SlotMachine(model);
}