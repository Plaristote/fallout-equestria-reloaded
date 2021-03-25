class Dialog {
  constructor(dialog) {
    this.dialog = dialog;
    this.dialog.mood = "smile";
    this.dialog.npc.setVariable("startRoutine", 2);
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
