export function onToggled(statistics, toggled) {
  console.log("ON TOGGLED MUTATED RAT", toggled);
  if (toggled) {
    statistics.traits.push("rat");
    statistics.traits.push("rat");
    statistics.strength     -= 3;
    statistics.perception   += 1;
    statistics.endurance    -= 3;
    statistics.charisma     -= 4;
    statistics.intelligence -= 3;
    statistics.agility      -= 1;
  }
  else {
    statistics.traits = [];
    statistics.strength     += 3;
    statistics.perception   -= 1;
    statistics.endurance    += 3;
    statistics.charisma     += 4;
    statistics.intelligence += 3;
    statistics.agility      += 1;
  }
}

export function modifyBaseStatistic(statistics, name, value) {
  if (name === "maxHitPoints")
    return value - 10;
  if (name === "carryWeight")
    return 5;
  return value;
}
