export function initialize() {
  game.onCityEnteredAt("stable-cavern", "demo-begin");
  game.worldmap.setPosition(150, 250);
  game.transitionRequired("intro-lyra-animation.mp4", 10);
}
