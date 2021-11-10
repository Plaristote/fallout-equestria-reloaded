import QtQuick 2.15

Repeater {
  property QtObject levelController: renderTarget.levelController
  id: root
  delegate: Image {
    id: dynamicObjectRenderer
    property QtObject dynamicObject: root.model[index]
    property point offset: root.levelController.getAdjustedOffsetFor(dynamicObject)

    source: fileProtocol + dynamicObject.spriteSource
    sourceClipRect: dynamicObject.clippedRect
    x: offset.x
    y: offset.y
    z: (dynamicObject.position.x + dynamicObject.position.y * renderTarget.mapSize.width)
    //Text { color: "yellow"; text: parent.z }

    Connections {
      target: dynamicObject
      function onSpritePositionChanged() {
        dynamicObjectRenderer.offset = root.levelController.getAdjustedOffsetFor(dynamicObject);
      }
    }
  }
}
