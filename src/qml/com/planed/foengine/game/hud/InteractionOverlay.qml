import QtQuick 2.15

Repeater {
  id: root
  property string   name
  property QtObject levelController
  property var      filter: function() { return true; }
  property bool     withColorOverlay: true
  property color    overlayColor:    Qt.rgba(255, 255, 0, 1)
  property color    overlayMaxColor: Qt.rgba(255, 255, 0, 0.5)
  property int      offsetX
  property int      offsetY
  property var      knownObjects: ({})
  property int      nextGeneration: 0

  delegate: Image {
    id: dynamicObjectLayer
    property QtObject dynamicObject: root.model[index]
    property point offset: levelController.getAdjustedOffsetFor(dynamicObject)

    opacity: 0
    Behavior on opacity {
      id: opacityBehavior
      NumberAnimation { duration: root.levelController.fadeDurationMs }
    }

    function updateVisibility() {
      dynamicObjectLayer.visible = root.filter(dynamicObject);
    }

    Component.onCompleted: {
      const path = dynamicObject.path;
      updateVisibility();
      if (root.knownObjects[path]) {
        root.knownObjects[path] = ++root.nextGeneration;
        opacityBehavior.enabled = false;
        opacity = 1;
        opacityBehavior.enabled = true;
      } else {
        root.knownObjects[path] = ++root.nextGeneration;
        opacity = 1;
      }
    }

    Component.onDestruction: {
      const capturedPath = dynamicObject.path;
      const knownObjects = root.knownObjects;
      const capturedGeneration = knownObjects[capturedPath];
      Qt.callLater(function() {
        if (knownObjects[capturedPath] === capturedGeneration)
          delete knownObjects[capturedPath];
      });
    }

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
    source: fileProtocol + dynamicObject.spriteSource
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
