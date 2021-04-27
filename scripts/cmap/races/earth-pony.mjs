export const isPlayable = true;

export function spriteSheet(model) {
  return {
    cloneOf:    "pony",
    base:       "pony",
    staticBase: "pony-eyes",
    color:      model.faceColor,
    hair:       model.hairTheme,
    hairColor:  model.hairColor
  };
}

export function onToggled(statistics, toggled) {
  var modifier = toggled ? 1 : -1;

  statistics.endurance += 1 * modifier;
  statistics.strength  += 1 * modifier;
}
