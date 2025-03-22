import QtQuick 2.15

Repeater {
  property QtObject levelController: renderTarget.levelController
  id: root
  delegate: Item {
    id: dynamicObjectRenderer
    property QtObject dynamicObject: root.model[index]
    property point offset: root.levelController.getAdjustedOffsetFor(dynamicObject)

    x: offset.x
    y: offset.y
    z: (dynamicObject.position.x + dynamicObject.position.y * renderTarget.mapSize.width) * 4 + dynamicObject.zIndex - 1

    Image {
      id: sprite
      source: fileProtocol + dynamicObject.spriteSource
      sourceClipRect: dynamicObject.clippedRect
      onSourceClipRectChanged: dynamicObjectRenderer.offset = levelController.getAdjustedOffsetFor(dynamicObject)
    }

    //Text { color: "yellow"; text: parent.z }

    DaylightShader {
      source:  sprite
      color:   root.levelController.ambientColor
      enabled: root.levelController.useAmbientLight
    }

    Connections {
      target: dynamicObject
      function onSpritePositionChanged() {
        dynamicObjectRenderer.offset = root.levelController.getAdjustedOffsetFor(dynamicObject);
      }
    }
  }
}
