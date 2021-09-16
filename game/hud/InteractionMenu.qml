import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Item {
  property QtObject levelController
  property QtObject interactionTarget
  property var levelCanvas
  property var interactionTypes: []
  property point interactionPosition
  property int bottomLimit: 0
  id: interactionMenu
  visible: interactionTarget && interactionTypes.length > 0

  x: interactionPosition.x
  y: {
    if (interactionPosition.y + menuColumn.height > bottomLimit)
      return interactionPosition.y = bottomLimit - menuColumn.height;
    return interactionPosition.y;
  }

  Column {
    id: menuColumn
    Repeater {
      model: interactionMenu.interactionTypes
      delegate: Button {
        property string interactionType: interactionMenu.interactionTypes[index]
        id: button
        hoverEnabled: true

        background: Image {
          property string bgType: parent.down ? 'pressed' : (parent.hovered ? 'active' : 'normal')
          source: "qrc:/assets/ui/interactions/" + interactionType + '-' + bgType + '.png'
        }

        onClicked:{
          if (interactionType === "use-skill") {
            actions.openSkilldex.trigger();
            skilldex.target = interactionMenu.interactionTarget;
          }
          else if (interactionType === "use-object") {
            itemPickerContainer.visible = true;
            itemPicker.target = interactionMenu.interactionTarget;
          }
          else
            levelController.interactOrderReceived(interactionMenu.interactionTarget, interactionMenu.interactionTypes[index]);
          interactionMenu.interactionTarget = null;
        }
      } // END BUTTON
    }
  }

  // INTERACTION
  Connections {
    target: levelController

    function onInteractionRequired(interactionTarget, interactionList) {
      if (interactionTarget)
      {
        interactionMenu.interactionTarget = interactionTarget;
        interactionMenu.interactionTypes  = interactionList;
        interactionMenu.interactionPosition = Qt.point(levelCanvas.mouseX, levelCanvas.mouseY)
      }
      else
        interactionMenu.interactionTarget = null;
    }

    function onMouseModeChanged() {
      interactionMenu.interactionTarget = null;
    }
  }
} // END INTERACTION MENU
