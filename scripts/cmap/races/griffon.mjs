export const isPlayable = false;

export const spriteSheet = "griffon";

export function onToggled(statistics, toggled) {
  let modifier = toggled ? 1 : -1;

  statistics.strength   += modifier;
  statistics.perception += modifier;
  statistics.endurance  += modifier;
  statistics.agility    += modifier;
}
