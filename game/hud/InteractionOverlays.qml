import QtQuick 2.15

Item {
  property QtObject levelController
  property var canvas

  Loader {
    sourceComponent: levelController.mouseMode > 0 && levelController.targetMode === 0 ? interactionOverlayComponent : null
  }

  Loader {
    sourceComponent: levelController.mouseMode > 0 ? characterInteractionOverlayComponent : characterOverlayComponent
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
    }
  }
}
