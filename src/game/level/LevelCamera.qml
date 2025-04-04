import QtQuick 2.15

Item {
  id: camera
  property QtObject levelController: parent.levelController
  property point origin
  onOriginChanged: levelController.canvasOffset = origin

  function getLimits() {
    const tilemap  = levelController.tilemap;
    const mapSize  = tilemap.mapSize;
    const tileSize = tilemap.tileSize;
    const rect     = tilemap.getLayer("ground").getRenderedRect();

    return {
      minX: -(mapSize.width * tileSize.width / 2) + camera.width / 2,
      maxY: camera.height / 2,
      maxX: (mapSize.height * tileSize.width / 2) + camera.width / 2,
      minY: -rect.height + camera.height / 2
    };
  }

  function translate(x, y) {
    const limits = getLimits();
    var   newOriginX = origin.x + x;
    var   newOriginY = origin.y + y;

    newOriginX = Math.min(newOriginX, limits.maxX);
    newOriginX = Math.max(newOriginX, limits.minX);
    newOriginY = Math.min(newOriginY, limits.maxY);
    newOriginY = Math.max(newOriginY, limits.minY);
    x = newOriginX - origin.x;
    y = newOriginY - origin.y;
    origin = Qt.point(newOriginX, newOriginY);
  }

  function moveToOffset(x, y) {
    translate(origin.x - x, origin.y - y);
  }

  function moveToCoordinates(x, y) {
    const position = levelController.tilemap.getPointFor(x, y);

    position.x -= canvas.width / 2
    position.y -= canvas.height / 2
    translate(-origin.x - position.x, -origin.y - position.y);
  }

  function moveToObject(object) {
    if (object) {
      const position = object.getSpritePosition();

      position.x -= camera.width / 2;
      position.y -= camera.height / 2;
      translate(-origin.x - position.x, -origin.y - position.y);
    }
  }

  Shortcut {
    sequence: "c"
    onActivated: moveToObject(levelController.player)
  }

  Connections {
    target: gamepad
    function onCameraAxisClicked() { moveToObject(levelController.player) }
  }

  Connections {
    target: levelController
    function onCameraFocusRequired(object) { moveToObject(object) }
  }

  Timer {
    id: afterLoadCameraCenter; running: true; interval: 100; onTriggered: camera.moveToObject(camera.levelController.player);
  }
}
