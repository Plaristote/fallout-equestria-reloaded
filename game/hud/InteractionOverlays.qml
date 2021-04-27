import QtQuick 2.15

Item {
  property QtObject levelController
  property var canvas
  property bool interactionMode: levelController.mouseMode === 1 || (levelController.mouseMode === 2 && levelController.targetMode === 0);
  property bool targetMode:      levelController.mouseMode === 2

  Loader {
    sourceComponent: interactionMode ? interactionOverlayComponent : null
  }

  Loader {
    sourceComponent: targetMode ? characterInteractionOverlayComponent : characterOverlayComponent
  }

  Component {
    id: interactionOverlayComponent
    InteractionOverlay {
      levelController: canvas.levelController
      controller:      canvas.controller
      model:           levelController.dynamicObjects
      filter:          function(item) {
        return !item.hidden && (item.getObjectType() !== "Character" || !item.isAlive());
      }
    }
  }

  Component {
    id: characterInteractionOverlayComponent
    InteractionOverlay {
      levelController:  canvas.levelController
      controller:       canvas.controller
      filter:           function(item) { return item.isAlive(); }
      model:            levelController.visibleCharacters
      overlayColor:     levelController.targetMode === 0 ? Qt.rgba(255, 255, 0, 1)   : Qt.rgba(255, 0, 0, 1)
      overlayMaxColor:  levelController.targetMode === 0 ? Qt.rgba(255, 255, 0, 0.5) : Qt.rgba(255, 0, 0, 0.5)
      layerOpacity: 0.6
    }
  }

  Component {
    id: characterOverlayComponent
    InteractionOverlay {
      levelController:  canvas.levelController
      controller:       canvas.controller
      filter:           function(item) { return item.isAlive(); }
      model:            levelController.visibleCharacters
      withColorOverlay: false
      layerOpacity: 0.6
    }
  }
}
