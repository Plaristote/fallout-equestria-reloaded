import QtQuick 2.15

Loader {
  id: pointerLoader
  property QtObject pointer: mouseCursor.virtualPointer
  sourceComponent: pointer ? virtualPointerDisplay : null
  z: 1999999999

  Component {
    id: virtualPointerDisplay
    Item {
      id: vpdi
      property QtObject pointer: pointerLoader.pointer
      x: mouseCursor.position.x - pointer.hotPoint.x
      y: mouseCursor.position.y - pointer.hotPoint.y

      Repeater {
        model: vpdi.pointer.frames
        delegate: Image {
          source: "qrc" + vpdi.pointer.frames[index]
          visible: index === vpdi.pointer.frameIndex
          width: vpdi.pointer.size.width
          height: vpdi.pointer.size.height
        }
      }
    }
  }
}
