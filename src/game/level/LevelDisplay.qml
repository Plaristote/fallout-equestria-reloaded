import QtQuick 2.15
import "../hud" as Hud

Item {
  id: root
  property QtObject gameController
  property QtObject levelController
  property alias renderTarget:   renderTarget
  property alias levelMouseArea: levelMouseArea
  property alias camera:         camera
  clip: true

  signal pickedObject(QtObject dynamicObject)
  signal pickedTile(int tileX, int tileY)

  onWidthChanged:  levelController.canvasSize.width  = width
  onHeightChanged: levelController.canvasSize.height = height

  LevelRenderTarget {
    id: renderTarget
    hoverTile: levelMouseArea.hoverTile
  }

  LevelMouseArea {
    id: levelMouseArea
    onHoverTileChanged: levelController.hoveredTile = hoverTile ? Qt.point(...hoverTile) : Qt.point(-1, -1)
  }

  LevelCamera {
    id: camera
    anchors.fill: parent
  }

  Hud.InteractionOverlays {
    levelController: parent.levelController
    offsetX: renderTarget.origin.x
    offsetY: renderTarget.origin.y
  }

  ScreenEdges {
    enabled: !parent.levelController.paused && !debugConsole.enabled
    onMoveTop:    { camera.translate(0, scrollSpeed); }
    onMoveLeft:   { camera.translate(scrollSpeed, 0); }
    onMoveRight:  { camera.translate(-scrollSpeed, 0); }
    onMoveBottom: { camera.translate(0, -scrollSpeed); }
  }
}
