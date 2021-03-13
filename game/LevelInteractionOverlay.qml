import QtQuick 2.15
import QtGraphicalEffects 1.15

Repeater {
  property QtObject levelController
  property var controller

  model: levelController.dynamicObjects
  delegate: Image {
    id: dynamicObjectLayer
    property QtObject dynamicObject: levelController.dynamicObjects[index]
    property point offset: controller.getAdjustedOffsetFor(dynamicObject)
    property bool  isCharacter: dynamicObject.getObjectType() === "Character"
    property bool  isDetected: isCharacter && level.visibleCharacters.indexOf(dynamicObject) >= 0
    property color overlayColor:    levelController.targetMode === 0 ? Qt.rgba(255, 255, 0, 1)   : Qt.rgba(255, 0, 0, 1)
    property color overlayMaxColor: levelController.targetMode === 0 ? Qt.rgba(255, 255, 0, 0.5) : Qt.rgba(255, 0, 0, 0.5)
    property int   overlayAnimState: 0
    property int   overlayAnimDuration: 500
    opacity: 0.3 + (isCharacter ? 0.3 : 0)

    ColorOverlay {
      anchors.fill: parent
      source: parent
      visible: levelController.mouseMode > 0 && levelController.player !== dynamicObject
      color: levelController.targetMode === 0 ? Qt.rgba(255, 255, 0, 255) : Qt.rgba(255, 0, 0, 255)

      ColorAnimation on color {
        from: dynamicObjectLayer.overlayColor
        to:   dynamicObjectLayer.overlayMaxColor
        running: dynamicObjectLayer.visible && dynamicObjectLayer.overlayAnimState === 0
        duration: dynamicObjectLayer.overlayAnimDuration
        onFinished: dynamicObjectLayer.overlayAnimState = 1
      }

      ColorAnimation on color {
        from: dynamicObjectLayer.overlayMaxColor
        to:   dynamicObjectLayer.overlayColor
        running: dynamicObjectLayer.visible && dynamicObjectLayer.overlayAnimState === 1
        duration: dynamicObjectLayer.overlayAnimDuration
        onFinished: dynamicObjectLayer.overlayAnimState = 0
      }
    }

    enabled: visible
    visible: isDetected || (!isCharacter && (levelController.mouseMode > 0 && levelController.targetMode === 0))
    source: "file:" + dynamicObject.spriteSource
    sourceClipRect: dynamicObject.clippedRect
    x: offset.x + origin.x
    y: offset.y + origin.y

    Connections {
      target: dynamicObject
      function onSpritePositionChanged() {
        dynamicObjectLayer.offset = controller.getAdjustedOffsetFor(dynamicObject);
      }
    }
  }
}
