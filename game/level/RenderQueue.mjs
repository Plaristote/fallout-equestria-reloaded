function isObjectVisible(object, detectedCharacters) {
  if (object.getObjectType() === "Character")
    return detectedCharacters.indexOf(object.objectName) >= 0;
  return !object.hidden;
}

function makeDetectionList(objects) {
  const names = [];

  for (var i = 0 ; i < objects.length ; ++i)
    names.push(objects[i].objectName);
  return names;
}

function insertInPositionQueue(queue, object) {
  var   inserted  = false;

  for (var i = 0 ; i < queue.length ; ++i) {
    if (object.getZIndex() <= queue[i].getZIndex()) {
      queue.splice(i, 0, object);
      inserted = true;
      break ;
    }
  }
  if (!inserted)
    queue.push(object);
  return queue;
}

export function initializeRenderQueue(mapSize) {
  const queue = [];

  for (var x = 0 ; x < mapSize.width ; ++x) {
    queue[x] = [];
    for (var y = 0 ; y < mapSize.height ; ++y)
      queue[x][y] = [];
  }
  return queue;
}

export function fillRenderQueue(renderQueue, level, viewAll) {
  const objects            = level.dynamicObjects;
  const detectedCharacters = makeDetectionList(level.visibleCharacters);

  for (var i = 0 ; i < objects.length ; ++i) {
    const object    = objects[i];
    const position  = object.position;
    const isValid   = position.x >= 0 && position.y >= 0 && object.floor === level.currentFloor;
    const isVisible = isValid && (viewAll || isObjectVisible(object, detectedCharacters));
    var   array     = renderQueue[position.x][position.y];

    if (!isVisible)
      continue ;
    renderQueue[position.x][position.y] = insertInPositionQueue(array, object);
  }
  return renderQueue;
}
