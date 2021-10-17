function initializePlayerInventory() {
  game.player.inventory.addItemOfType("stable-suit", 1);
  game.player.inventory.addItemOfType("health-potion", 2);
}

export function initialize() {
  game.worldmap.setPosition(420, 230);
  initializePlayerInventory();
  game.onCityEnteredAt("tutorial", "demo-begin");
  game.quests.addQuest("tutorial");
  //game.onCityEnteredAt("city-sample", "");
  //game.transitionRequired("intro-lyra-animation.mp4", 10);

  //game.player.addBuff("bleeding");
}
