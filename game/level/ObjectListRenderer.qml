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
    z: (dynamicObject.position.x + dynamicObject.position.y * renderTarget.mapSize.width) * 4 + dynamicObject.zIndex - 1
    //Text { color: "yellow"; text: parent.z }

    onSourceClipRectChanged: offset = levelController.getAdjustedOffsetFor(dynamicObject)

    Connections {
      target: dynamicObject
      function onSpritePositionChanged() {
        dynamicObjectRenderer.offset = root.levelController.getAdjustedOffsetFor(dynamicObject);
      }
    }

    DaylightShader {
      source: parent
      color: renderTarget.levelController.ambientColor
    }
  }
}
