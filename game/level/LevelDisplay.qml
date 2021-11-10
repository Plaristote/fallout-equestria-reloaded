import QtQuick 2.15
import "../hud" as Hud

Item {
  id: root
  property QtObject gameController
  property QtObject levelController
  property alias renderTarget:   renderTarget_
  property alias levelMouseArea: levelMouseArea_
  property alias camera:         camera_
  clip: true

  signal pickedObject(QtObject dynamicObject)
  signal pickedTile(int tileX, int tileY)

  LevelRenderTarget {
    id: renderTarget_
    hoverTile: levelMouseArea.hoverTile
  }

  LevelMouseArea {
    id: levelMouseArea_
    onHoverTileChanged: levelController.hoveredTile = hoverTile ? Qt.point(...hoverTile) : Qt.point(-1, -1)
  }

  LevelCamera {
    id: camera_
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
