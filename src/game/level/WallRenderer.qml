import QtQuick 2.15

Item {
  id: wallRenderer
  property QtObject levelController
  property int tx: index % renderTarget.mapSize.width
  property int ty: Math.round(index / renderTarget.mapSize.width)
  readonly property point renderPosition: levelController.getRenderPositionForTile(tx, ty)
  readonly property var block: renderTarget.blocks ? renderTarget.blocks.getTile(tx, ty) : null
  readonly property var vwall: renderTarget.vwalls ? renderTarget.vwalls.getTile(tx, ty) : null
  readonly property var hwall: renderTarget.hwalls ? renderTarget.hwalls.getTile(tx, ty) : null
  property bool rendered: isRendered(levelController.renderedTiles)
  function isRendered() { return levelController.isCaseRendered(tx, ty); }

  property bool withClippingH: false
  property bool withClippingV: false

  x: renderPosition.x
  y: renderPosition.y
  z: (tx + ty * renderTarget.mapSize.width) * 4 + (vwall || hwall ? 2 : 0)
  visible: rendered

  //Text { color: "white"; font.bold: true; text: parent.z }
  Loader {
    readonly property var wall: block;
    sourceComponent: wall && rendered ? wallComponent : null;
    x: wall ? renderTarget.tileSize.width - block.clippedRect.width : 0
    y: wall ? renderTarget.tileSize.height - block.clippedRect.height : -renderTarget.wallHeight
  }
  Loader {
    readonly property var wall: vwall;
    sourceComponent: wall && rendered ? wallComponent : null;
    y: -renderTarget.wallHeight
  }
  Loader {
    readonly property var wall: hwall;
    sourceComponent: wall && rendered ? wallComponent : null;
    x: wall ? renderTarget.tileSize.width - hwall.clippedRect.width : 0
    y: wall ? renderTarget.tileSize.height - hwall.clippedRect.height : -renderTarget.wallHeight
  }

  Component {
    id: wallComponent
    Rectangle {
      width: image.width
      height: image.height
      color: "transparent"
      visible: renderTarget.renderWalls

      Image {
        id: image
        source: fileProtocol + wall.image
        sourceClipRect: wall.clippedRect
        visible: !(gameManager.withPlayerCropCircle || wallRenderer.levelController.useAmbientLight)
      }

      PlayerCropCircle {
        id: pcc
        source: image
        player: wallRenderer.levelController.player
        offzetX: wallRenderer.x - (image.width - renderTarget.tileSize.width)
        offzetY: wallRenderer.y - (image.height - renderTarget.wallHeight) - renderTarget.tileSize.height
        withClipping: (wallRenderer.withClippingH && wallRenderer.withClippingV)
                   || (wallRenderer.withClippingH && wall === hwall)
                   || (wallRenderer.withClippingV && wall === vwall)
      }

      DaylightShader {
        source:  gameManager.withPlayerCropCircle ? pcc : image
        color:   wallRenderer.levelController.ambientColor
        enabled: wallRenderer.levelController.useAmbientLight
      }
    }
  }
}
