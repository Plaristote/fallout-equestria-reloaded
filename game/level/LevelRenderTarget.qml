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
      id: groundLayer
      source: `file:///${levelController.preRenderPath}floor${levelController.currentFloor}_tilemap.png`
      x:      groundRect.x
      y:      groundRect.y
      width:  groundRect.width
      height: groundRect.height
    }

    Repeater {
      model: levelController.tilemap.zones
      delegate: Item {
        property QtObject zone: levelController.tilemap.zones[index]
        visible: zone.type === "exit" || visibleZones.indexOf(zone) >= 0
        Repeater {
          model: zone.positionCount
          delegate: Image {
            property point    position: zone.getPositionAt(index)
            property QtObject tile: ground.getTile(position.x, position.y)
            property point    renderPosition: tile ? tile.renderPosition : Qt.point(0, 0)
            source: "qrc:/assets/tilesets/zones.png"
            sourceClipRect: zone.clippedRect
            x: renderPosition.x
            y: renderPosition.y
          }
        }
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
      delegate: Item {
        property int tx: index % renderTarget.mapSize.width
        property int ty: Math.round(index / renderTarget.mapSize.width)
        property point renderPosition: levelController.getRenderPositionForTile(tx, ty)
        property var block: renderTarget.blocks ? renderTarget.blocks.getTile(tx, ty) : null
        property var vwall: renderTarget.vwalls ? renderTarget.vwalls.getTile(tx, ty) : null
        property var hwall: renderTarget.hwalls ? renderTarget.hwalls.getTile(tx, ty) : null

        x: renderPosition.x
        y: renderPosition.y
        z: tx + ty * renderTarget.mapSize.width

        //Text { color: "white"; font.bold: true; text: parent.z }
        Loader { property var wall: block; sourceComponent: wall ? wallComponent : null; y: -renderTarget.wallHeight }
        Loader { property var wall: vwall; sourceComponent: wall ? wallComponent : null; y: -renderTarget.wallHeight }
        Loader { property var wall: hwall; sourceComponent: wall ? wallComponent : null; y: -renderTarget.wallHeight }
      }
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
      delegate: Image {
        property QtObject roof: renderTarget.levelController.tilemap.roofs[index]
        property rect renderRect: roof.getRenderedRect()
        property bool isFloor: roof.name.startsWith("floor_")

        source: `file:///${levelController.preRenderPath}floor${renderTarget.levelController.floor}_roof_${roof.name}.png`
        visible: renderTarget.renderRoofs
        opacity: roof.visible ? 1 : 0
        x:       renderRect.x + (isFloor ? -width / 2 : 0)
        y:       renderRect.y + (isFloor ? -renderTarget.wallHeight : 0)
        z:       99999999
        width:   renderRect.width
        height:  renderRect.height

        Behavior on opacity { NumberAnimation { duration: 300 } }
      }
    }

    CursorRenderer {
      id: cursorRenderer
      levelController: root.levelController
    }
  } // RenderItems

  Component {
    id: wallComponent
    Image {
      source: fileProtocol + wall.image
      sourceClipRect: wall.clippedRect
      visible: renderTarget.renderWalls
    }
  }
}
