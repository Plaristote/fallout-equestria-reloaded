var controller;

export function initialize(param) {
  controller = param;
  console.log("SCRIPT DIALOG initialize", param);
  return "my-entry-point";
}

export function unhappy() {
  console.log("Still haz controller?", controller);
  controller.mood = "angry";
  return "insults";
}

export function happy() {
  controller.mood = "smile";
  return "thanks";
}

export function unavailable() {
  return false;
}

export function available() {
  return true;
}
