export function findPathTo(actions, target) {
  var lowestCost = -1;
  var choice;

  for (var x = -1 ; x <= 1 ; ++x) {
    for (var y = -1 ; y <= 1 ; ++y) {
      const tx = target.x + x;
      const ty = target.y + y;
      const apCost = actions.getMovementApCost(tx, ty);

      //console.log("Trying path", tx, ty, "cost", apCost, "currentLowest", lowestCost);
      if (apCost >= 0 && (lowestCost < 0 || apCost < lowestCost)) {
        lowestCost = apCost;
        choice = { x: tx, y: ty };
        if (apCost == 0)
          break ;
      }
    }
  }
  return { ap: lowestCost, position: choice };
}

export function areInContact(pos1, pos2) {
  const distX = Math.max(pos1.x, pos2.x) - Math.min(pos1.x, pos2.x);
  const distY = Math.max(pos1.y, pos2.y) - Math.min(pos1.y, pos2.y);

  console.log("Pathfinding.areInContact:", pos1, pos2, distX, distY);
  return distX <= 1 && distY <= 1;
}

export function getDistance(pos1, pos2) {
  const distX = Math.max(pos1.x, pos2.x) - Math.min(pos1.x, pos2.y);
  const distY = Math.max(pos1.y, pos2.y) - Math.min(pos1.y, pos2.y);

  return Math.sqrt(distX * distX + distY * distY);
}

export function getRandomCaseInZone(zone) {
  if (zone && zone.getPositionCount() > 0) {
    const positionIt = Math.floor(Math.random() * 100) % zone.getPositionCount();

    return zone.getPositionAt(positionIt);
  }
  return {x: 0, y: 0};
}
