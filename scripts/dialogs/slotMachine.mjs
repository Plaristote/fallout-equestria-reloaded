import {skillCheck} from "../cmap/helpers/checks.mjs";

class Dialog {
  constructor(dialog) {
    this.dialog = dialog;
  }

  capsCount() { return game.player.inventory.count("bottlecaps"); }

  onIntro() {
    if (this.gains === 0)
      return i18n.t("dialogs.slotMachine.lost");
    else if (this.gains > 0)
      return i18n.t("dialogs.slotMachine.won", {amount: this.gains});
    return null;
  }

  bet(amount) {
    const target = 95 + Math.ceil(Math.random() * 50);
    if (skillCheck(game.player, "gambling", {target: target})) {
      this.gains = amount * 5;
      game.player.inventory.addItemOfType("bottlecaps", this.gains - amount);
    }
    else {
      game.player.inventory.removeItemOfType("bottlecaps", amount);
      this.gains = 0;
    }
  }

  bet10()  { this.bet(10); }
  bet20()  { this.bet(20); }
  bet50()  { this.bet(50); }
  bet100() { this.bet(100); }
  has10()  { return this.capsCount() >= 10; }
  has20()  { return this.capsCount() >= 20; }
  has50()  { return this.capsCount() >= 50; }
  has100() { return this.capsCount() >= 100; }
}

export function create(dialog) {
  return new Dialog(dialog);
}
