export const spriteSheet = "scorpion";

export const itemSlots = {"use-1":  "any"};

export function getDefaultItem(model, slot) {
  return "melee-stinger";
}

export function onToggled(statistics, toggled) {
  if (toggled) {
    statistics.traits.push("scorpion");
    statistics.traits.push("scorpion");
    statistics.strength     -= 3;
    statistics.perception   -= 1;
    statistics.endurance    -= 3;
    statistics.charisma     -= 4;
    statistics.intelligence -= 3;
    statistics.agility      -= 1;
  }
  else {
    statistics.traits = [];
    statistics.strength     += 3;
    statistics.perception   += 1;
    statistics.endurance    += 3;
    statistics.charisma     += 4;
    statistics.intelligence += 3;
    statistics.agility      += 1;
  }
}

export function modifyBaseStatistic(statistics, name, value) {
  if (name === "maxHitPoints")
    return value - 15;
  if (name === "carryWeight")
    return 5;
  return value;
}
