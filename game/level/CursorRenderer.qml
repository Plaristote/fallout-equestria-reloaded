import QtQuick 2.15

Loader {
  id: cursorRenderer
  property QtObject levelController
  property point    renderPosition: {
    if (renderTarget.hoverTile)
      return levelController.getRenderPositionForTile(...renderTarget.hoverTile);
    return Qt.point(0, 0);
  }
  x: renderPosition.x
  y: renderPosition.y
  z: 99999999
  sourceComponent: {
    if (levelController.mouseMode === 0)
      return levelController.isPlayerTurn ? combatPathComponent : tileCursorComponent;
    else if (levelController.mouseMode === 2 && levelController.targetMode === 2)
      return tileZoneComponent;
    return null;
  }

  Component {
    id: tileCursorComponent
    Image {
      source: "qrc:/assets/ui/cursors/move-tile.png"
    }
  }

  Component {
    id: tileZoneComponent
    Item {
      property int zoneRadius: levelController.activeItem.zoneSize
      property int zoneSize: (zoneRadius * 2 + 1)
      Repeater {
        model: zoneSize * zoneSize
        delegate: Image {
          property int   zoneX: -zoneSize + Math.floor(index / zoneSize)
          property int   zoneY: -zoneSize + index % zoneSize
          property var   tile: [
            renderTarget.hoverTile[0] + zoneX + zoneSize - zoneRadius,
            renderTarget.hoverTile[1] + zoneY + zoneSize - zoneRadius
          ]
          property point renderPosition: levelController.getRenderPositionForTile(...tile)
          x: cursorRenderer.x - renderPosition.x
          y: cursorRenderer.y - renderPosition.y
          source: "qrc:/assets/ui/cursors/target-tile.png"
        }
      }
    }
  }

  Component {
    id: combatPathComponent
    Item {
      id: combatPathElement
      property var path: getPath()
      property int length: path.positions.length

      function getPath() {
        return levelController.previewPathTo(...renderTarget.hoverTile);
      }

      Connections {
        target: levelController.player
        function onActionPointsChanged() { combatPathElement.path = combatPathElement.getPath(); }
      }

      Connections {
        target: renderTarget
        function onHoverTileChanged() { combatPathElement.path = combatPathElement.getPath(); }
      }

      Repeater {
        model: combatPathElement.length
        delegate: Image {
          property var   tile: [
            combatPathElement.path.positions[index].x,
            combatPathElement.path.positions[index].y
          ]
          property bool lastTile: index === (combatPathElement.length - 1)
          property point renderPosition: levelController.getRenderPositionForTile(...tile)
          x: renderPosition.x - cursorRenderer.x
          y: renderPosition.y - cursorRenderer.y
          source: "qrc:/assets/ui/cursors/move-tile" + (lastTile ? "" : "-unavailable") + ".png"
          Component.onCompleted: {
            console.log("kewa", combatPathElement.path.costs[index]);
          }

          Text {
            anchors.centerIn: parent
            text:             combatPathElement.path.costs[index].toString()
            font.pointSize:   application.consoleFont.bigSize
            font.family:      application.consoleFont.name
            style:            Text.Outline
            styleColor:       "black"
            color:            "white"
          }
        }
      }

      Loader {
        sourceComponent: path.length > length || path.length === 0 ? tileCursorComponent : null
      }
    }
  }
}
