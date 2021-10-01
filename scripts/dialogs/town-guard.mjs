class Dialog {
  constructor(dialog) {
    this.dialog = dialog;
    this.dialog.mood = "angry";
  }
  
  onSurrender() {
    const alarmedCharacters = this.dialog.npc.parent.objects;
    
    for (var i = 0 ; i < alarmedCharacters ; ++i) {
      const character = alarmedCharacters[i].getScriptObject();
      character.alarmComponent.cancelAlarmSignal();
    }
    this.sendPlayerToJail();
  }
  
  sendPlayerToJail() {
    const jail = level.findGroup("police-station.jail");
    const cell = Math.floor(Math.random() * 101) % Math.max(1, jail.groups.length);
    
    if (jail.groups[cell]) {
      level.moveCharacterToZone(level.player, jail.groups[cell].controlZone);
      level.cameraFocusRequired(level.player);
    }
  }

  onFight() {
    this.dialog.npc.setAsEnemy(level.player);
    if (!level.isInCombat(this.dialog.npc))
      level.joinCombat(this.dialog.npc);
  }
}

export function create(dialog) {
  return new Dialog(dialog);
}
