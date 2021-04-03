import QtQuick 2.15
import QtGraphicalEffects 1.15

Repeater {
  id: root
  property QtObject levelController
  property var      controller
  property var      filter: function() { return true; }
  property bool     withColorOverlay: true
  property color    overlayColor:    Qt.rgba(255, 255, 0, 1)
  property color    overlayMaxColor: Qt.rgba(255, 255, 0, 0.5)

  delegate: Image {
    id: dynamicObjectLayer
    property QtObject dynamicObject: root.model[index]
    property point offset: controller.getAdjustedOffsetFor(dynamicObject)
    property bool  isCharacter: dynamicObject.getObjectType() === "Character"

    opacity: 0.3 + (isCharacter ? 0.3 : 0)

    Timer {
      running: root.visible
      interval: 500
      onTriggered: dynamicObjectLayer.visible = root.filter(dynamicObject)
    }

    InteractionColorOverlay {
      from:    overlayColor
      to:      overlayMaxColor
      source:  parent
      visible: withColorOverlay
    }

    enabled: visible
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
