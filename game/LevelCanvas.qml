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
    function onTilemapReady() {
      preloadImages();
      initializeRenderer();
    }
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

  LevelInteractionOverlay {
    levelController: canvas.levelController
    controller:      canvas.controller
    model:           levelController.dynamicObjects
    filter:          function(item) { return item.getObjectType() !== "Character" || !item.isAlive(); }
    visible:         levelController.mouseMode > 0 && levelController.targetMode === 0
  }

  LevelInteractionOverlay {
    levelController: canvas.levelController
    controller:      canvas.controller
    filter:          function(item) { return item.isAlive(); }
    model:           levelController.visibleCharacters
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
      pathPrefix: fileProtocol,
      rootPath: rootPath
    });
  }

  function preloadImages() {
    const images = [];
    var i;

    for (i = 0 ; i < levelController.tilemap.textureList.length ; ++i) {
      const texturePath = levelController.tilemap.textureList[i];
      if (images.indexOf(texturePath) === -1) {
        console.log("LevelCanvas: Preloading image", texturePath, "from Tiled resources");
        loadImage("../" + texturePath);
        images.push(texturePath);
      }
      loadImage("../assets/ui/cursors/move-tile.png");
    }
    unloadImage(rootPath + "_tilemap.png");
    loadImage(rootPath + "_tilemap.png");
    for (i = 0 ; i < levelController.tilemap.roofs.length ; ++i) {
      const roof = levelController.tilemap.roofs[i];
      const src  = rootPath + "_roof_" + roof.name + ".png";
      unloadImage(src);
      loadImage(src);
    }
    for (i = 0 ; i < levelController.tilemap.lights.length ; ++i) {
      const lights = levelController.tilemap.lights[i];
      const src = rootPath + "_lights_" + lights.name + ".png";
      unloadImage(src);
      loadImage(src);
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
