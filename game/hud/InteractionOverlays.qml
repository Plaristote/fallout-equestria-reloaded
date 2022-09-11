import QtQuick 2.15
import "../Interaction.mjs" as Interaction;

Item {
  id: root
  property QtObject levelController
  property bool interactionMode: Interaction.shouldDisplayInteractionLayer(levelController.mouseMode, levelController.targetMode)
  property bool targetMode:      Interaction.shouldDisplayTargetLayer(levelController.mouseMode, levelController.targetMode)
  property int offsetX
  property int offsetY

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
      levelController: root.levelController
      model:           levelController.visibleObjects
      filter:          function(item) { return item.hasInteractionOverlay(); }
      offsetX:         root.offsetX
      offsetY:         root.offsetY
    }
  }

  Component {
    id: characterInteractionOverlayComponent
    InteractionOverlay {
      levelController:  root.levelController
      filter:           function(item) { return item.floor === levelController.currentFloor && item.isAlive() && item !== levelController.player; }
      model:            levelController.visibleCharacters
      overlayColor:     levelController.targetMode === Interaction.TargetMode.Any ? Qt.rgba(255, 255, 0, 1)   : Qt.rgba(255, 0, 0, 1)
      overlayMaxColor:  levelController.targetMode === Interaction.TargetMode.Any ? Qt.rgba(255, 255, 0, 0.5) : Qt.rgba(255, 0, 0, 0.5)
      offsetX:          root.offsetX
      offsetY:          root.offsetY
    }
  }

  Component {
    id: characterOverlayComponent
    InteractionOverlay {
      levelController:  root.levelController
      filter:           function(item) { return item.floor === levelController.currentFloor && item.isAlive() }
      model:            levelController.visibleCharacters
      withColorOverlay: false
      offsetX:          root.offsetX
      offsetY:          root.offsetY
    }
  }
}
