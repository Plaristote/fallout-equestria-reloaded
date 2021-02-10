export function initialize(character) {
  console.log("----> dummy initialize <-----");
  //character.setTickBehavior(1000, true);
}

export function getAvailableInteractions() {
  return ["talk-to", "look", "use-skill"];
}

export function onMovementEnded(character) {
  console.log("dummy js: movement ended", character.position.x, character.position.y);
}

export function onReachedDestination(character) {
  console.log("dummy.js: reached destination", character.position.x, character.position.y);
}

export function onTicked(character) {
  console.log("----> dummy tick <------")
}
