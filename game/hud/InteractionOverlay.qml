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

    function updateVisibility() {
      dynamicObjectLayer.visible = root.filter(dynamicObject);
    }

    Component.onCompleted: updateVisibility()
    onSourceClipRectChanged: offset = controller.getAdjustedOffsetFor(dynamicObject)

    Timer {
      running: root.visible
      interval: 500
      onTriggered: dynamicObjectLayer.updateVisibility();
    }

    Loader {
      anchors.fill: parent
      sourceComponent: root.withColorOverlay ? colorOverlay : null
    }

    Component {
      id: colorOverlay
      InteractionColorOverlay {
        from:    root.overlayColor
        to:      root.overlayMaxColor
        source:  dynamicObjectLayer
        visible: root.withColorOverlay
      }
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
