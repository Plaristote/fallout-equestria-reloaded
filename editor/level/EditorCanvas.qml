import QtQuick 2.15
import "../../game" as GameComponents
import "EditorRender.mjs" as LevelRender

GameComponents.LevelCanvas {
  property QtObject editorObject
  property bool showHoverCoordinates: false
  property bool renderRoofs: true
  property bool renderWalls: true
  property bool editingZone: false

  signal pickedObject(QtObject dynamicObject)
  signal pickedTile(int tileX, int tileY)
  signal toggleZoneTile(int tileX, int tileY)

  function initializeRenderer() {
    if (levelController) {
      controller = new LevelRender.EditorController(canvas, {
        level: levelController,
        tilemap: levelController.tilemap,
        pathPrefix: fileProtocol,
        rootPath: rootPath
      });
    }
    else
      console.log("LevelEditorCanvas: called initializeRenderer with unset levelController");
  }

  Text {
    anchors.top: parent.top; anchors.right: parent.right
    color: "white"
    text: hoverTile !== null ? hoverTile[0] + '/' + hoverTile[1] : ''
    visible: showHoverCoordinates
  }
}
