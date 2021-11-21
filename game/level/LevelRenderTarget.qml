import QtQuick 2.15
import QtQuick.Controls 2.15

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
  property bool     renderWalls: true
  property bool     renderRoofs: true
  property var      visibleZones: []
  property alias    groundLayer: groundLayer_

  id: renderTarget
  color: "yellow"
  width:  groundLevelLayer.getRenderedRect().width
  height: groundLevelLayer.getRenderedRect().height
  x: levelController.canvasOffset.x - width / 2
  y: levelController.canvasOffset.y

  Behavior on x { NumberAnimation { duration: 250 } }
  Behavior on y { NumberAnimation { duration: 250 } }

  Item {
    x: parent.width / 2

    Image {
      id: groundLayer_
      source: `file:///${levelController.preRenderPath}floor${levelController.currentFloor}_tilemap.png`
      x:      groundRect.x
      y:      groundRect.y
      width:  groundRect.width
      height: groundRect.height

      DaylightShader {
        source: groundLayer
        color: renderTarget.levelController.ambientColor
      }
    }

    Repeater {
      model: levelController.tilemap.zones
      delegate: ZoneRenderer {
        zone: levelController.tilemap.zones[index]
        tilesetSource: "qrc:/assets/tilesets/zones.png"
      }
    }

    ObjectListRenderer {
      model: renderTarget.levelController.visibleObjects
    }

    ObjectListRenderer {
      model: renderTarget.levelController.visibleCharacters
    }

    Repeater {
      id: wallsRenderer
      model: renderTarget.mapSize.width * renderTarget.mapSize.height
      delegate: WallRenderer {}
    }

    Repeater {
      model: renderTarget.levelController.visualEffects
      delegate: Image {
        property QtObject sprite: renderTarget.levelController.visualEffects[index]
        x: sprite.spritePosition.x
        y: sprite.spritePosition.y
        source: fileProtocol + sprite.spriteSource
        sourceClipRect: sprite.clippedRect
      }
    }

    Repeater {
      model: renderTarget.levelController.tilemap.roofs
      delegate: Rectangle {
        property QtObject roof: renderTarget.levelController.tilemap.roofs[index]
        property rect renderRect: roof.getRenderedRect()
        property bool isFloor: roof.name.startsWith("floor_")

        visible: renderTarget.renderRoofs
        opacity: roof.visible ? 1 : 0
        x:       renderRect.x + (isFloor ? -width / 2 : 0)
        y:       renderRect.y + (isFloor ? -renderTarget.wallHeight : 0)
        z:       99999999
        width:   renderRect.width
        height:  renderRect.height
        color: "transparent"

        Behavior on opacity { NumberAnimation { duration: 300 } }

        Image {
          id: mypic
          source: `file:///${levelController.preRenderPath}floor${renderTarget.levelController.currentFloor}_roof_${roof.name}.png`
          visible: false
        }

        PlayerCropCircle {
          source: mypic
        }
      }
    }

    CursorRenderer {
      id: cursorRenderer
      levelController: root.levelController
    }
  } // RenderItems
}
