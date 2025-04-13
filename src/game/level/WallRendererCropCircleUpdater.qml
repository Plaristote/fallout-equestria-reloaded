import QtQuick

QtObject {
  property QtObject player
  property var wallMap: [];
  property int range: 8
  property point playerPosition: player.position
  property point lastPlayerPosition: playerPosition

  // TODO: this function is unused.
  // It is apparently slower than the previous implementation.
  function updateCulling(culledIn, culledOut) {
    for (let tile of culledIn) {
      if (wallMap[tile.x] && wallMap[tile.x][tile.y])
        wallMap[tile.x][tile.y].rendered = true;
    }
    for (let tile of culledOut) {
      if (wallMap[tile.x] && wallMap[tile.x][tile.y])
        wallMap[tile.x][tile.y].rendered = false;
    }
  }

  function updateClipping(element, position) {
    let withClippingH = false;
    let withClippingV = false;

    if (element.tx >= position.x && element.ty >= position.y)
      withClippingH = withClippingV = true;
    else {
      if (element.tx >= position.x)
        withClippingV = true;
      if (element.ty >= position.y)
        withClippingH = true;
    }
    if (element.withClippingH != withClippingH)
      element.withClippingH = withClippingH;
    if (element.withClippingV != withClippingV)
     element.withClippingV = withClippingV;
  }

  function updateWallsForPosition(position) {
    const tilesToUpdate = new Set();
    const currentRegion = {
      minX: position.x - range,
      maxX: position.x + range,
      minY: position.y - range,
      maxY: position.y + range
    };
    const lastRegion = {
      minX: lastPlayerPosition.x - range,
      maxX: lastPlayerPosition.x + range,
      minY: lastPlayerPosition.y - range,
      maxY: lastPlayerPosition.y + range
    };

    function addRegionTiles(region) {
      for (let x = Math.max(0, region.minX); x <= region.maxX; x++) {
        if (!wallMap[x]) continue;

        for (let y = Math.max(0, region.minY); y <= region.maxY; y++) {
          if (wallMap[x][y]) {
            tilesToUpdate.add(wallMap[x][y]);
          }
        }
      }
    }

    addRegionTiles(currentRegion);
    addRegionTiles(lastRegion);

    tilesToUpdate.forEach(element => {
      updateClipping(element, position);
      //element.triggerPlayerCropCircleUpdate();
    });

    lastPlayerPosition = position;
  }

  onPlayerPositionChanged: updateWallsForPosition(player.position);
}
