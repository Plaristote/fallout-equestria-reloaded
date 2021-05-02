import QtQuick 2.15
import "../Interaction.mjs" as Interaction;

Item {
  property QtObject levelController
  property var canvas
  property bool interactionMode: Interaction.shouldDisplayInteractionLayer(levelController.mouseMode, levelController.targetMode)
  property bool targetMode:      Interaction.shouldDisplayTargetLayer(levelController.mouseMode, levelController.targetMode)

  opacity: 0.6

  Loader {
    sourceComponent: interactionMode ? interactionOverlayComponent : null
  }

  Loader {
    sourceComponent: targetMode || interactionMode ? characterInteractionOverlayComponent : characterOverlayComponent
  }

  Component {
    id: interactionOverlayComponent
    InteractionOverlay {
      levelController: canvas.levelController
      controller:      canvas.controller
      model:           levelController.dynamicObjects
      filter:          function(item) {
        return !item.hidden && item.hasInteractionOverlay();
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
      overlayColor:     levelController.targetMode === Interaction.TargetMode.Any ? Qt.rgba(255, 255, 0, 1)   : Qt.rgba(255, 0, 0, 1)
      overlayMaxColor:  levelController.targetMode === Interaction.TargetMode.Any ? Qt.rgba(255, 255, 0, 0.5) : Qt.rgba(255, 0, 0, 0.5)
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
