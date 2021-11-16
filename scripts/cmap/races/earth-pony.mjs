export const isPlayable = true;

export const withFaceColor = true;

export const faces = ["mare-basic", "stallion-basic"];

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

export const itemSlots = {
  "armor":  "armor",
  "saddle": "saddle",
  "use-1":  "any",
  "use-2":  "any"
};

export function getDefaultItem(model, slot) {
  switch (slot) {
  case "use-1":
    return "melee-hoofercut";
  case "use-2":
    return "melee-bucking";
  }
  return "";
}

export function onToggled(statistics, toggled) {
  var modifier = toggled ? 1 : -1;

  statistics.endurance += 1 * modifier;
  statistics.strength  += 1 * modifier;
}

export function modifyBaseSkill(characterSheet, name, value) {
  switch (name) {
  case "outdoorsman":
    return value + 25;
  }
  return value;
}
