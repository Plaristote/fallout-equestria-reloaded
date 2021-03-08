import QtQuick 2.15
import "LevelRender.mjs" as LevelRender

Canvas {
  id: canvas
  anchors.fill: parent
  renderStrategy: Canvas.Cooperative
  renderTarget:   Canvas.Image
  onPaint: { if (controller) { controller.render(); } }

  property QtObject levelController;
  property point origin;
  property var controller;
  property var hoverTile: [];
  property bool hoverTileHintVisible: false
  property QtObject hoveredObject
  property bool hoveredObjectEnabled: levelController.mouseMode === 2

  Component.onCompleted: {
    preloadImages();
  }

  onLevelControllerChanged: {
    preloadImages();
    initializeRenderer();
  }

  Timer {
    id: afterLoadCameraCenter; interval: 100; onTriggered: moveToObject(levelController.player);
  }

  Connections {
    target: levelController
    function onUpdated() { canvas.requestPaint(); }
    function onCameraFocusRequired(object) { moveToObject(object); }
  }

  onAvailableChanged: {
    initializeRenderer();
    afterLoadCameraCenter.running = true;
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    enabled: !levelController.paused
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    onClicked: {
      if (mouse.button === Qt.RightButton)
        levelController.swapMouseMode();
      else
        controller.onMouseClick(mouse, mouseX, mouseY);
    }
    onMouseXChanged: onMouseMoved()
    onMouseYChanged: onMouseMoved()
    cursorShape: enabled ? Qt.BlankCursor : Qt.ArrowCursor

    function onMouseMoved() {
      hoverTile = controller.getHoveredCase(mouseX - canvas.origin.x, mouseY - canvas.origin.y);
      if (hoveredObjectEnabled)
        hoveredObject = controller.getHoveredObject(mouseX - canvas.origin.x, mouseY - canvas.origin.y);
      else
        hoveredObject = null;
    }
  }

  LevelInteractionOverlay {
    levelController: canvas.levelController
    controller:      canvas.controller
  }

  LevelCursor {
    id: levelCursor
    visible: mouseArea.containsMouse && (mouseMode !== 0 || !canvas.hoverTileHintVisible)
    mouseMode: levelController.mouseMode
    mouseX: mouseArea.mouseX
    mouseY: mouseArea.mouseY

    UsageSuccessHint {
      levelController: canvas.levelController
      target: hoveredObject
    }
  }

  function initializeRenderer() {
    controller = new LevelRender.Controller(canvas, {
      level: levelController,
      tilemap: levelController.tilemap,
      pathPrefix: fileProtocol
    });
  }

  function preloadImages() {
    const images = [];

    for (var i = 0 ; i < levelController.tilemap.textureList.length ; ++i) {
      const texturePath = levelController.tilemap.textureList[i];
      if (images.indexOf(texturePath) === -1) {
        console.log("LevelCanvas: Preloading image", texturePath, "from Tiled resources");
        loadImage("../" + texturePath);
        images.push(texturePath);
      }
      loadImage("../assets/ui/cursors/move-tile.png");
    }

    animationLibrary.getSources().forEach(function(texture) {
      console.log("LevelCanvas: Preloading texture", texture, "from animation library");
      loadImage("../assets/sprites/" + texture);
    });
  }

  function translate(x, y) {
    const context = getContext("2d");
    const limits = controller.getLimits();
    var   newOriginX = origin.x + x;
    var   newOriginY = origin.y + y;

    newOriginX = Math.min(newOriginX, limits.maxX);
    newOriginX = Math.max(newOriginX, limits.minX);
    newOriginY = Math.min(newOriginY, limits.maxY);
    newOriginY = Math.max(newOriginY, limits.minY);
    x = newOriginX - origin.x;
    y = newOriginY - origin.y;
    context.translate(x, y);
    origin.x += x;
    origin.y += y;
    requestPaint();
  }

  function moveToOffset(x, y) {
    translate(origin.x - x, origin.y - y);
  }

  function moveToCoordinates(x, y) {
    const position = controller.getPointFor(x, y);

    position.x -= canvas.width / 2
    position.y -= canvas.height / 2
    translate(-origin.x - position.x, -origin.y - position.y);

  }

  function moveToObject(object) {
    const position = object.getSpritePosition();

    position.x -= canvas.width / 2
    position.y -= canvas.height / 2
    translate(-origin.x - position.x, -origin.y - position.y);
  }

  Shortcut {
    sequence: "c"
    onActivated: moveToObject(levelController.player)
  }
}
