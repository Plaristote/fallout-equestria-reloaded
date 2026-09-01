import QtQuick 2.15
import Game 1.0

Rectangle {
  property QtObject levelController: parent.levelController
  property QtObject tilemap:    levelController.tilemap
  property size     mapSize:    tilemap.mapSize
  property size     tileSize:   tilemap.tileSize
  property var      hoverTile
  property point    origin:     Qt.point(x + width / 2, y)
  property alias    renderWalls: renderer.renderWalls
  property alias    renderRoofs: renderer.renderRoofs
  property alias    visibleZones: renderer.visibleZones

  id: renderTarget
  color: "black"
  x: levelController.canvasOffset.x - width / 2
  y: levelController.canvasOffset.y

  Behavior on x { NumberAnimation { duration: 250 } }
  Behavior on y { NumberAnimation { duration: 250 } }

  LevelRenderer {
    id: renderer
    levelController: parent.levelController
    originX: renderTarget.width / 2
  }

  CursorRenderer {
    id: cursorRenderer                                                                                                           
    levelController: root.levelController                                                                                        
  } 
}
