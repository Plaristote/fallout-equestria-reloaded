import QtQuick 2.15
import "LevelRender.mjs" as LevelRender

Canvas {
  id: canvas
  anchors.fill: parent
  onImageLoaded: imageReady = true
  onPaint: { if (controller) { controller.render(); } }

  property QtObject levelController;
  property point origin;
  property var controller;
  property bool imageReady;

  Component.onCompleted: {
    preloadImages();
  }

  Timer {
    running: canvas.imageReady
    repeat: true
    interval: 17
    onTriggered: canvas.requestPaint()
  }

  onAvailableChanged: {
    controller = new LevelRender.Controller(canvas, { level: levelController, tilemap: levelController.tilemap });
    requestPaint();
  }

  MouseArea {
    anchors.fill: parent
    onClicked: {
      controller.onMouseClick(mouse, mouseX, mouseY);
    }
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
    }

    animationLibrary.getSources().forEach(function(texture) {
      console.log("LevelCanvas: Preloading texture", texture, "from animation library");
      loadImage("../assets/sprites/" + texture);
    });
  }

  function translate(x, y) {
    const context = getContext("2d");
    const limits = controller.getLimits();
    /*
    if ((origin.x + x < limits.minX && x < 0) || (origin.x + x > limits.maxX && x > 0))
      x = 0;
    if ((origin.y + y < limits.minY && y < 0) || (origin.y + y > limits.maxY && y > 0))
      y = 0;
    */
    context.translate(x, y);
    origin.x += x;
    origin.y += y;
    //console.log("View offset:", origin.x, origin.y, JSON.stringify(limits));
    requestPaint();
  }

  function moveToOffset(x, y) {
    translate(origin.x - x, origin.y - y);
  }

  function moveToCoordinates(x, y) {
    const offset = controller.getPointFor(x, y);
    moveToOffset(offset.x - canvas.width / 2, offset.y - canvas.height / 2);
  }
}
