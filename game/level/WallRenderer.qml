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

  x: renderPosition.x
  y: renderPosition.y
  z: (tx + ty * renderTarget.mapSize.width) * 3 + (vwall || hwall ? 2 : 0)
  visible: rendered

  //Text { color: "white"; font.bold: true; text: parent.z }
  Loader { readonly property var wall: block; sourceComponent: wall && rendered ? wallComponent : null; y: -renderTarget.wallHeight }
  Loader { readonly property var wall: vwall; sourceComponent: wall && rendered ? wallComponent : null; y: -renderTarget.wallHeight }
  Loader { readonly property var wall: hwall; sourceComponent: wall && rendered ? wallComponent : null; y: -renderTarget.wallHeight }

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
        visible: false
      }

      PlayerCropCircle {
        source: image
        levelController: wallRenderer.levelController
        offzetX: wallRenderer.x
        offzetY: wallRenderer.y - renderTarget.wallHeight
        onPositionRefreshed: {
          withClipping = levelController.player &&
            ((tx >= levelController.player.position.x && ty >= levelController.player.position.y) ||
             (wall === hwall && tx === levelController.player.position.x - 1 && ty === levelController.player.position.y) ||
             (wall === vwall && ty === levelController.player.position.y - 1 && tx === levelController.player.position.x));
        }
      }
    }
  }
}
