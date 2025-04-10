pragma ComponentBehavior: Bound
import QtQuick 2.15

Item {
  id: wallRenderer
  property QtObject levelController
  property int tx
  property int ty
  readonly property point renderPosition: levelController.getRenderPositionForTile(tx, ty)
  readonly property var block: renderTarget.blocks ? renderTarget.blocks.getTile(tx, ty) : null
  readonly property var vwall: renderTarget.vwalls ? renderTarget.vwalls.getTile(tx, ty) : null
  readonly property var hwall: renderTarget.hwalls ? renderTarget.hwalls.getTile(tx, ty) : null
  property bool rendered: false
  property bool withClippingH: false
  property bool withClippingV: false

  signal triggerPlayerCropCircleUpdate();

  x: renderPosition.x
  y: renderPosition.y
  z: (tx + ty * renderTarget.mapSize.width) * 4 + (vwall || hwall ? 2 : 0)
  visible: rendered && renderTarget.renderWalls

  //Text { color: "white"; font.bold: true; text: parent.z }
  Loader {
    readonly property var wall: block;
    sourceComponent: wall ? wallComponent : null;
    x: wall ? renderTarget.tileSize.width - block.clippedRect.width : 0
    y: wall ? renderTarget.tileSize.height - block.clippedRect.height : -renderTarget.wallHeight
  }
  Loader {
    readonly property var wall: vwall;
    sourceComponent: wall ? wallComponent : null;
    y: -renderTarget.wallHeight
  }
  Loader {
    readonly property var wall: hwall;
    sourceComponent: wall ? wallComponent : null;
    x: wall ? renderTarget.tileSize.width - hwall.clippedRect.width : 0
    y: wall ? renderTarget.tileSize.height - hwall.clippedRect.height : -renderTarget.wallHeight
  }

  Component {
    id: wallComponent
    Rectangle {
      readonly property var wall: parent.wall
      property alias withClipping: pcc.withClipping
      property alias cropCircleCenter: pcc.centerPoint
      property alias offset: pcc.offset
      width: image.width
      height: image.height
      color: "transparent"

      Component.onCompleted: {
        const controller = wallRenderer.parent.controller;
        if      (wall === vwall) controller.vWallRenderer = this;
        else if (wall === hwall) controller.hWallRenderer = this;
        else if (wall === block) controller.blockRenderer = this;
      }

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
        autoRefresh: false
        withClipping: false

        Connections {
          target: wallRenderer
          function onTriggerPlayerCropCircleUpdate() {
            pcc.updateCenter();
            pcc.positionRefreshed();
          }
        }
      }

      DaylightShader {
        source:  gameManager.withPlayerCropCircle ? pcc : image
        color:   wallRenderer.levelController.ambientColor
        enabled: wallRenderer.levelController.useAmbientLight
      }
    }
  }
}
