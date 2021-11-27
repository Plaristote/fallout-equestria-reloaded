import QtQuick 2.15

Item {
  property QtObject tileLayer
  visible: tileLayer.visible

  Repeater {
    model: tileLayer.tiles.length
    delegate: Image {
      property QtObject tile: tileLayer.tiles[index]
      property point    renderPosition: tile ? tile.renderPosition : Qt.point(0, 0)
      source: fileProtocol + tile.image
      sourceClipRect: tile.clippedRect
      x: renderPosition.x
      y: renderPosition.y
    }
  }
}
