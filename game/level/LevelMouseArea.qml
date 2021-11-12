import QtQuick 2.15

MouseArea {
  property QtObject levelController: parent.levelController
  property QtObject hoveredObject
  property bool hoveredObjectEnabled: levelController.mouseMode === 1 || levelController.mouseMode === 2
  property var hoverTile

  id: mouseArea
  anchors.fill: parent
  acceptedButtons: Qt.LeftButton | Qt.RightButton
  hoverEnabled: true
  onMouseXChanged: mouseRefreshTimer.running = true
  onMouseYChanged: mouseRefreshTimer.running = true
  onContainsMouseChanged: levelController.mouseInMap = containsMouse
  onPressed: {
    console.log("debug: MouseArea clicked", pressedButtons, pressedButtons & Qt.LeftButton, pressedButtons & Qt.RightButton);
    if (pressedButtons & Qt.LeftButton)
      onLeftButtonClick();
    else if (pressedButtons & Qt.RightButton)
      onRightButtonClick();
  }
  Component.onCompleted: {
    if (mouseX !== 0 && mouseY !== 0)
      levelController.mouseInMap = true;
  }

  Timer {
    interval: 1500
    running: true
    onTriggered: {
      if (mouseX !== 0 && mouseY !== 0)
        levelController.mouseInMap = true;
    }
  }

  Timer {
    id: mouseRefreshTimer
    interval: 53
    repeat: false
    onTriggered: mouseArea.onMouseMoved()
  }

  function onLeftButtonClick() {
    switch (levelController.mouseMode) {
      case 0:
        onTileClick();
        break ;
      case 1:
        onObjectClick();
        break ;
      case 2:
        onTargetClick();
        break ;
    }
  }

  function onRightButtonClick() {
    levelController.swapMouseMode();
  }

  function onTileClick() {
    if (hoverTile) {
      mouseArea.parent.pickedTile(...hoverTile);
      return true;
    }
    return false;
  }

  function onObjectClick() {
    const object = levelController.getObjectAt(...getCursorCoordinates());

    if (object)
      mouseArea.parent.pickedObject(object);
    return object != null;
  }

  function onTargetClick() {
    switch (levelController.targetMode) {
    case 0:
      if (!onObjectClick())
        onTileClick();
      break ;
    case 1:
      onObjectClick();
      break ;
    case 2:
      onTileClick();
      break ;
    }
  }

  function getCursorCoordinates() {
    return [
      mouseX - levelController.canvasOffset.x,
      mouseY - levelController.canvasOffset.y
    ];
  }

  function onMouseMoved() {
    const newHoveredTile = getHoveredCase(...getCursorCoordinates());

    cursorShape = Qt.BlankCursor;
    if (!hoverTile || !newHoveredTile || hoverTile[0] !== newHoveredTile[0] || hoverTile[1] !== newHoveredTile[1])
      hoverTile = newHoveredTile;
    hoveredObject = hoveredObjectEnabled
      ? levelController.getObjectAt(...getCursorCoordinates())
      : null;
  }

  function getHoveredCase(posX, posY) {
    const x = Math.floor(posX / renderTarget.tileSize.width) + Math.floor(posY / renderTarget.tileSize.height);

    //for (var x = 0 ; x < renderTarget.mapSize.width; ++x) {
      for (var y = 0 ; y < renderTarget.mapSize.height; ++y) {
        const pos = renderTarget.tilemap.getPointFor(x, y);

        if (posX >= pos.x && posX <= pos.x + renderTarget.tileSize.width &&
            posY >= pos.y && posY <= pos.y + renderTarget.tileSize.height)
          return [x, y];
      }
    //}
    return null;
  }
}
