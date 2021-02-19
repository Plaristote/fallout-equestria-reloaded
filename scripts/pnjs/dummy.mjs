import {CharacterBehaviour} from "./character.mjs";

class Dummy extends CharacterBehaviour {
  constructor(model) {
    super(model);
    this.dialog = "parley";
  }

  getAvailableInteractions() {
    console.log("Wtf");
    return super.getAvailableInteractions();
  }
}

export function create(model) {
  return new Dummy(model);
}

/*
export var dialog = "parley";

var self = null;
var bubbleN = 0;
var bubbleDuration = 5000;
const bubbleMessage = [
  "Bonjour, ceci est mon premier message",
  "Hey, ho, deuxième message-o",
  "Ok bon c'est fini maintenant"
];
const bubbleColors = ["white", "yellow", "red"];

class Character {
  constructor(model) {
    this.model = model;
  }
};

export function onTalkTo() {
  level.initializeDialog(self);
}

export function initialize(character) {
  self = character;
  console.log("----> dummy initialize <-----", character.hasVariable("initialized"));
  if (!character.hasVariable("initialized")) {
    character.tasks.addLocalTask("taskExecuted", bubbleDuration, bubbleMessage.length);
    character.tasks.addLocalTask("wanderAround", 3000, -1);
    character.setVariable("initialized", true);
    console.log("--> prepared recurrent tasks");
  } else {
    console.log("--> recurrent tasks already initialized");
  }
  console.log("----> dummy initialize DONE <-----");
}

export function taskExecuted(character) {
  console.log("Executing task for character", character);

  game.level.addTextBubble(character, bubbleMessage[bubbleN], bubbleDuration, bubbleColors[bubbleN]);
  bubbleN++;
  return true;
}

export function wanderAround(character) {
  const position = character.getPosition;

  position.x = Math.floor(Math.random() * 9);
  position.y = Math.floor(Math.random() * 9);
  game.level.moveTo(character, position.x, position.y);
  console.log("Wander around task ran", position.x, position.y, character.getVariable("wanderAround"));
  character.setVariable("wanderAround", character.getVariable("wanderAround") + 1);
  return true;
}

export function getAvailableInteractions() {
  if (!self.isAlive())
    return ["use", "look"];
  return ["talk-to", "look", "use-skill"];
}

export function onMovementEnded(character) {
  console.log("dummy js: movement ended", character.position.x, character.position.y);
}

export function onReachedDestination(character) {
  console.log("dummy.js: reached destination", character.position.x, character.position.y);
}
*/
