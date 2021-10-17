class Dialog {
  constructor(dialog) {
    this.dialog = dialog;
    this.dialog.mood = "neutral";
    this.dialog.npc.setVariable("startRoutine", 2);
  }

  getEntryPoint() {
    var state = 0;
    var entryPoints = ["entry", "reentry", "victory"];

    if (this.dialog.npc.hasVariable("entryPoint"))
      state = this.dialog.npc.getVariable("entryPoint");
    return entryPoints[state];
  }

  onEntryPoint() {
    this.dialog.npc.setVariable("entryPoint", 1);
  }

  onHelpAsked() {
    this.dialog.mood = "smile";
    if (this.dialog.player.statistics.strength >= 5) {
      this.dialog.npc.scriptCall("joinedPlayer", "1");
      return i18n.t("dialogs.tutoCharIntro.accept-to-help");
    }
    return i18n.t("dialogs.tutoCharIntro.decline-to-help");
  }

  onHostileExit() {
    this.dialog.npc.setAsEnemy(this.dialog.player);
    return "";
  }

  onHostile() {
    this.dialog.mood = "angry";
  }

  onNeutral() {
    this.dialog.mood = "neutral";
  }
}

export function create(dialog) {
  return new Dialog(dialog);
}
