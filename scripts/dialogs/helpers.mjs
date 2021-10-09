export class DialogHelper {
  constructor(dialog) {
    this.dialog = dialog;
    if (level && level.getScriptObject().ambiance)
      this.dialog.ambiance = level.getScriptObject().ambiance;
    //this.updateMood();
  }

  getDefaultMood() {
    return "neutral";
  }

  updateMood() {
    if (this.dialog.npc.hasVariable("mood"))
      this.dialog.mood = this.dialog.npc.getVariable("mood");
    else
      this.dialog.mood = this.getDefaultMood();
  }

  setPersistentMood(mood) {
    if (mood === this.getDefaultMood())
      this.dialog.npc.unsetVariable("mood");
    else
      this.dialog.npc.setVariable("mood", mood);
    this.updateMood();
  }
}
