import QtQuick 2.15
import QtGraphicalEffects 1.15

Repeater {
  property QtObject levelController
  property var controller

  model: levelController.dynamicObjects
  delegate: Rectangle {
    id: lightObjectOverlay
    property QtObject dynamicObject: levelController.dynamicObjects[index]
    property point offset: controller.getAdjustedOffsetFor(dynamicObject)
    property var lightLayer: null

    color: "transparent"
    x: offset.x + origin.x
    y: offset.y + origin.y
    visible: lightLayer !== null && lightLayer.visible

    onLightLayerChanged: updateVisibility()
    Component.onCompleted: lightObjectOverlay.updateLightLayer()

    function updateLightLayer() {
      for (var i = 0 ; i < levelController.tilemap.lights.length ; ++i) {
        const layer = levelController.tilemap.lights[i];

        if (layer.isInside(dynamicObject.position.x, dynamicObject.position.y)) {
          lightLayer = layer;
          return ;
        }
      }
      lightLayer = null;
    }

    function updateVisibility() {
      console.log("Visibility for", dynamicObject.objectName, ":", lightLayer && lightLayer.visible, dynamicObject.isVisible);
      visible = lightLayer && lightLayer.visible && dynamicObject.isVisible;
    }

    Connections {
      target: lightLayer
      function onVisibleChanged() {
        lightObjectOverlay.updateVisibility();
      }
    }

    Connections {
      target: dynamicObject

      function onSpritePositionChanged() {
        lightObjectOverlay.offset = controller.getAdjustedOffsetFor(dynamicObject);
      }

      function onPositionChanged() {
        lightObjectOverlay.updateLightLayer();
      }

      function onVisibilityChanged() {
        lightObjectOverlay.updateVisibility();
      }
    }

    Image {
      id: lightObjectBase
      visible: false
      source: "file:" + dynamicObject.spriteSource
      sourceClipRect: dynamicObject.clippedRect
    }

    Rectangle {
      id: lightObjectFilling
      visible: false
      width: lightObjectBase.width
      height: lightObjectBase.height
      color: Qt.rgba(1, 1, 1, 0.5)
    }

    OpacityMask {
      anchors.fill: lightObjectFilling
      source: lightObjectFilling
      maskSource: lightObjectBase
    }
  }
}
