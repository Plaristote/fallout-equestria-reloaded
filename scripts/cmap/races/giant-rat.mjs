export const spriteSheet = "giant-rat";

export const faces = ["giant-rat"];

export function onToggled(statistics, toggled) {
  if (toggled) {
    statistics.traits.push("rat");
    statistics.perception   -= 1;
    statistics.charisma     -= 4;
    statistics.intelligence -= 3;
    statistics.agility      += 1;
  }
  else {
    statistics.traits = [];
    statistics.perception   += 1;
    statistics.charisma     += 4;
    statistics.intelligence += 3;
    statistics.agility      -= 1;
  }
}
