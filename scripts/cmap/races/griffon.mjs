export const isPlayable = true;

export const faces = ["griffon", "griffon-alt"];

export function spriteSheet(model) {
  return {
    cloneOf: "griffon",
    base:    "griffon",
    overlay: "griffon-wings",
    hair:    model.hairTheme
  };
}

export function onToggled(statistics, toggled) {
  let modifier = toggled ? 1 : -1;

  statistics.setFaceColor(188, 188, 188, 1);
  statistics.strength   += modifier;
  statistics.perception += modifier;
  statistics.endurance  += modifier;
  statistics.agility    += modifier;
}
