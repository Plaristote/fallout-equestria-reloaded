import QtQuick 2.15

Repeater {
  id: root
  property QtObject levelController
  property var      filter: function() { return true; }
  property bool     withColorOverlay: true
  property color    overlayColor:    Qt.rgba(255, 255, 0, 1)
  property color    overlayMaxColor: Qt.rgba(255, 255, 0, 0.5)
  property int      offsetX
  property int      offsetY

  delegate: Image {
    id: dynamicObjectLayer
    property QtObject dynamicObject: root.model[index]
    property point offset: levelController.getAdjustedOffsetFor(dynamicObject)

    function updateVisibility() {
      dynamicObjectLayer.visible = root.filter(dynamicObject);
    }

    Component.onCompleted: updateVisibility()
    onSourceClipRectChanged: offset = levelController.getAdjustedOffsetFor(dynamicObject)

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
    x: offset.x + root.offsetX
    y: offset.y + root.offsetY

    Connections {
      target: dynamicObject
      function onSpritePositionChanged() {
        dynamicObjectLayer.offset = levelController.getAdjustedOffsetFor(dynamicObject);
      }
    }
  }
}
