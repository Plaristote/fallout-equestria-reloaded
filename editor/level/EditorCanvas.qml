import QtQuick 2.15
import "../../game/level" as LevelComponents

LevelComponents.LevelDisplay {
  property QtObject editorObject
  property bool showHoverCoordinates: false
  property bool renderRoofs: true
  property bool renderWalls: true
  property bool editingZone: false
  property var hoverTile: renderTarget.hoverTile

  onEditorObjectChanged: {
    if (editorObject && editorObject.controlZone)
      renderTarget.visibleZones = [editorObject.controlZone];
    else
      renderTarget.visibleZones = [];
  }

  Text {
    anchors.top: parent.top; anchors.right: parent.right
    color: "white"
    text: hoverTile !== null ? hoverTile[0] + '/' + hoverTile[1] : ''
    visible: showHoverCoordinates
  }
}
