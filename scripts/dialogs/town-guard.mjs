export class GuardDialog {
  constructor(dialog) {
    this.dialog = dialog;
    this.dialog.mood = "angry";
  }

  get npcScript() {
    return this.dialog.npc.getScriptObject();
  }

  get jail() {
    return this.npcScript.jailComponent ? this.npcScript.jailComponent.jail : null;
  }

  arrestationText() {
    if (this.jail && this.jail.shouldPlayerBeInJail())
      return this.dialog.t("back-to-cell");
    return this.dialog.t("arrest");
  }

  onSurrender() {
    const alarmedCharacters = this.npcScript.squad;
    
    for (var i = 0 ; i < alarmedCharacters ; ++i) {
      const character = alarmedCharacters[i].getScriptObject();
      character.alarmComponent.cancelAlarmSignal();
    }
    this.sendPlayerToJail();
  }

  sendPlayerToJail() {
    if (this.jail) {
      this.jail.sendToRandomCell(level.player);
      this.jail.sentencePlayer(60*60*24*1000);
      level.cameraFocusRequired(level.player);
      game.appendToConsole(i18n.t("messages.jail-sentence", { duration: 1 }));
    }
    else
      console.log("/!\\ cannot send player to cell: jail object not found.");
  }

  onFight() {
    this.dialog.npc.setAsEnemy(level.player);
    if (!level.isInCombat(this.dialog.npc))
      level.joinCombat(this.dialog.npc);
  }

  onBarterStart() {
    this.dialog.text = i18n.t("messages.wont-barter");
    return false;
  }
}

export function create(dialog) {
  return new GuardDialog(dialog);
}
