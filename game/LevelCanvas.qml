import QtQuick 2.15
import "LevelRender.mjs" as LevelRender
import "./hud" as Hud

Canvas {
  id: canvas
  anchors.fill: parent
  renderStrategy: Canvas.Immediate
  renderTarget:   Canvas.Image
  onPaint: { if (controller) { controller.render(); } }

  property QtObject levelController;
  property point origin;
  property var controller;
  property var hoverTile: [];
  property bool hoverTileHintVisible: false
  property QtObject hoveredObject
  property bool hoveredObjectEnabled: levelController.mouseMode === 2
  property alias mouseX: mouseArea.mouseX
  property alias mouseY: mouseArea.mouseY

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
    function onTilemapReady() {
      preloadImages();
      initializeRenderer();
    }
    function onUpdated() { canvas.requestPaint(); }
    function onCameraFocusRequired(object) { moveToObject(object); }
  }

  onAvailableChanged: {
    console.log("(!) LevelCanvas.available changed:", available);
    if (available) {
      initializeRenderer();
      afterLoadCameraCenter.running = true;
    }
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    onClicked: {
      if (mouse.button === Qt.RightButton)
        levelController.swapMouseMode();
      else if (!levelController.paused)
        controller.onMouseClick(mouse, mouseX, mouseY);
    }
    onContainsMouseChanged: {
      levelController.mouseInMap = containsMouse;
    }

    onMouseXChanged: onMouseMoved()
    onMouseYChanged: onMouseMoved()
    cursorShape: enabled ? Qt.BlankCursor : Qt.ArrowCursor

    function onMouseMoved() {
      hoverTile = controller.getHoveredCase(mouseX - canvas.origin.x, mouseY - canvas.origin.y);
      if (hoveredObjectEnabled)
        hoveredObject = levelController.getObjectAt(mouseX - canvas.origin.x, mouseY - canvas.origin.y);
      else
        hoveredObject = null;
    }
  }

  LevelLightOverlay {
    levelController: canvas.levelController
    controller:      canvas.controller
    model:           canvas.levelController.dynamicObjects
  }

  Hud.InteractionOverlays {
    canvas: parent
    levelController: parent.levelController
  }

  Item {
    id: levelCursor
    property real mouseX: mouseArea.mouseX
    property real mouseY: mouseArea.mouseY

    Image {
      visible: !hoverTileHintVisible && levelController.mouseMode === 0
      source: "qrc:/assets/ui/cursors/move-tile.png"
      height: 36
      width: 72
      x: mouseX - 18
      y: mouseY - 36
    }

    UsageSuccessHint {
      levelController: canvas.levelController
      target: hoveredObject
      targetTile: hoverTile
    }
  }

  function initializeRenderer() {
    controller = new LevelRender.Controller(canvas, {
      level: levelController,
      tilemap: levelController.tilemap,
      pathPrefix: fileProtocol,
      rootPath: rootPath
    });
  }

  function preloadImages() {
    const images = [];
    const preRenderPath = "file:///" + levelController.preRenderPath;
    var i;

    for (i = 0 ; i < levelController.tilemap.textureList.length ; ++i) {
      const texturePath = levelController.tilemap.textureList[i];
      if (images.indexOf(texturePath) === -1) {
        loadImage("../" + texturePath);
        images.push(texturePath);
      }
      loadImage("../assets/ui/cursors/move-tile.png");
      loadImage("../assets/ui/cursors/target-tile.png");
    }
    unloadImage(preRenderPath + "tilemap.png");
    loadImage(preRenderPath + "tilemap.png");
    for (i = 0 ; i < levelController.tilemap.roofs.length ; ++i) {
      const roof = levelController.tilemap.roofs[i];
      const src  = preRenderPath + "roof_" + roof.name + ".png";
      unloadImage(src);
      loadImage(src);
    }
    for (i = 0 ; i < levelController.tilemap.lights.length ; ++i) {
      const lights = levelController.tilemap.lights[i];
      const src = preRenderPath + "lights_" + lights.name + ".png";
      unloadImage(src);
      loadImage(src);
    }

    animationLibrary.getSources().forEach(function(texture) {
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

  Connections {
    target: gamepad
    function onCameraAxisClicked() { moveToObject(levelController.player) }
  }
}
