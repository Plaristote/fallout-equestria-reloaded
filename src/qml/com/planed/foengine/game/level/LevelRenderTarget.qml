import QtQuick 2.15
import Game 1.0

Rectangle {
  property QtObject levelController: parent.levelController
  property QtObject baseLayer:  levelController.getFloorGrid(0).getTilemap().getLayer("ground")
  property QtObject tilemap:    levelController.tilemap
  property size     mapSize:    tilemap.mapSize
  property size     tileSize:   tilemap.tileSize
  property QtObject ground:     tilemap.getLayer("ground")
  property QtObject blocks:     tilemap.getLayer("blocks")
  property QtObject vwalls:     tilemap.getLayer("walls-v")
  property QtObject hwalls:     tilemap.getLayer("walls-h")
  property rect     groundRect: ground.getRenderedRect()
  property var      hoverTile
  property point    origin:     Qt.point(x + width / 2, y)
  property real     wallHeight: tileSize.height * 2
  property alias    renderWalls: renderer.renderWalls
  property alias    renderRoofs: renderer.renderRoofs
  property alias    visibleZones: renderer.visibleZones
  property rect     renderedTiles: levelController.renderedTiles

  id: renderTarget
  color: "black"
  width:  groundRect.width
  height: groundRect.height
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
