import QtQuick 2.15

Item {
  id: root
  property QtObject tileLayer
  visible: tileLayer.visible
  opacity: 0
  Component.onCompleted: updateOpacity()

  function updateOpacity() { root.opacity = tileLayer.visible ? 1 : 0; }

  Behavior on opacity { NumberAnimation { duration: 300 } }

  Connections {
    target: tileLayer
    function onVisibleChanged() { root.updateOpacity(); }
  }

  Loader {
    //sourceComponent: tileLayer.prerendered ? prerenderedRenderer : dynamicRenderer
    sourceComponent: dynamicRenderer
  }

  Component {
    id: prerenderedRenderer
    Image {
      property rect renderRect: tileLayer.getRenderedRect()
      x:       renderRect.x
      y:       renderRect.y
      width:   renderRect.width
      height:  renderRect.height
      source: `file:///${levelController.preRenderPath}floor${renderTarget.levelController.currentFloor}_lights_${tileLayer.name}.png`
    }
  }

  Component {
    id: dynamicRenderer
    Repeater {
      model: tileLayer.tiles.length
      delegate: Loader {
        property QtObject tile: tileLayer.tiles[index]
        sourceComponent: tile ? tileRenderer : null

        Component {
          id: tileRenderer
          Image {
            source: fileProtocol + tile.image
            sourceClipRect: tile.clippedRect
            x: tile.renderPosition.x
            y: tile.renderPosition.y
          }
        }
      }
    }
  }
}
