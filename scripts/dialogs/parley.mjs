export class Dialog {
  constructor(model) {
    this.model = model;
  }

  triggerMyEntryPoint() {
    if (this.model.npc.getVariable("introduced") === true)
      return i18n.t("dialogs.parley.greetings-alt");
    this.model.npc.setVariable("introduced", true);
    return i18n.t("dialogs.parley.greetings");
  }

  showSomethingCool() {
    this.model.mood = "sad";
    return "somethingCool";
  }

  changeColor() {
    this.model.mood = "smile";
    this.model.npc.statistics.setFaceColor(225, 225, 0, 175);
  }

  putHatOn() {
    this.model.npc.statistics.faceAccessories.push("fancypants");
  }

  notAvailable() {
    return false;
  }
};

export function create(model) {
  return new Dialog(model);
}
