import QtQuick 2.15

Item {
  property QtObject zone
  property string   tilesetSource
  visible: zone.type === "exit" || visibleZones.indexOf(zone) >= 0

  Repeater {
    model: zone.positionCount
    delegate: Image {
      id: zoneTile
      property point    position: zone.getPositionAt(index)
      property QtObject tile: ground.getTile(position.x, position.y)
      property point    renderPosition: tile ? tile.renderPosition : Qt.point(0, 0)
      source: tilesetSource
      sourceClipRect: zone.clippedRect
      x: renderPosition.x
      y: renderPosition.y

      Connections {
        target: zone
        function onTilesChanged() {
          zoneTile.position = zone.getPositionAt(index);
        }
      }
    }
  }
}
