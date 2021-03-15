export const isPlayable = true;

export function onToggled(statistics, toggled) {
  var modifier = toggled ? 1 : -1;

  statistics.endurance += 1 * modifier;
  statistics.strength  += 1 * modifier;
}
