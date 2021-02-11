var controller;

export function initialize(param) {
  controller = param;
  console.log("SCRIPT DIALOG initialize", param);
  controller.npc.statistics.faceTheme = "mare-basic";
  controller.npc.statistics.setFaceColor(255, 255, 0, 150);
  controller.npc.statistics.faceAccessories = ["eye-scar"];
  return "my-entry-point";
}

export function unhappy() {
  console.log("Still haz controller?", controller);
  controller.mood = "angry";
  controller.npc.statistics.setFaceColor(255, 0, 0, 150);
  return "insults";
}

export function happy() {
  controller.mood = "smile";
  controller.npc.statistics.faceAccessories.push("fancypants");
  return "thanks";
}

export function unavailable() {
  return false;
}

export function available() {
  return true;
}
