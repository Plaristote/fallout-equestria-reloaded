export function getValueFromRange(from, to) {
  return from + Math.floor(Math.random() * (to + 1));
}

function isJinxed(character) {
  return character.statistics.traits.indexOf("jinxed") >= 0;
}

export function randomCheck(threshold, handlers) {
  const criticalThreshold = isJinxed(level.player) ? 5 : 15;
  const roll = getValueFromRange(0, 100);

  if (roll >= (100 - criticalThreshold) && handlers.criticalFailure)
    return handlers.criticalFailure();
  else if (roll >= threshold && handlers.failure)
    return handlers.failure();
  else if (handlers.success)
    return handlers.success();
  return null;
}
