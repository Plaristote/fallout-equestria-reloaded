export const withFaceColor = true;

export const faces = ["gator-pony"];

export function spriteSheet(model) {
  return {
    cloneOf:    "gator-pony",
    base:       "gator-pony",
    staticBase: "gator-pony-eyes",
    color:      model.faceColor
  };
}

export function onToggled(statistics, toggled) {
  var modifier = toggled ? 1 : -1;

  statistics.strength  += (1 * modifier);
  statistics.charisma  -= (1 * modifier);
}

export function modifyBaseSkill(characterSheet, name, value) {
  switch (name) {
  case "outdoorsman":
    return value + 25;
  }
  return value;
}
