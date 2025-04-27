import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Item {
  property QtObject levelController
  property QtObject interactionTarget
  property var mouseArea
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

  onVisibleChanged: {
    buttonNavigation.currentIndex = 0;
    if (visible && typeof soundManager != "undefined") {
      soundManager.play("ui/int-menu");
    }
  }

  UiStyle.ButtonNavigation {
    id: buttonNavigation
    buttonRepeater: repeater
    currentIndex: 0
  }

  Column {
    id: menuColumn
    Repeater {
      id: repeater
      model: interactionMenu.interactionTypes
      delegate: Button {
        property string interactionType: interactionMenu.interactionTypes[index]
        id: button
        hoverEnabled: true

        background: Image {
          property string bgType: parent.down ? 'pressed' : (parent.hovered ? 'active' : 'normal')
          source: "qrc:/assets/ui/interactions/" + interactionType + '-' + bgType + '.png'
        }

        onDownChanged: {
          if (typeof soundManager != "undefined") {
            soundManager.play(down ? "ui/push-button-in" : "ui/push-button-out");
          }
        }

        onClicked:{
          if (interactionType === "use-skill") {
            actions.openSkilldex.trigger();
            skilldex.target = interactionMenu.interactionTarget;
          }
          else if (interactionType === "use-object") {
            itemPickerContainer.state = "shown";
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
      if (interactionTarget) {
        interactionMenu.interactionTarget = interactionTarget;
        interactionMenu.interactionTypes  = interactionList;
        interactionMenu.interactionPosition = Qt.point(mouseArea.mouseX, mouseArea.mouseY)
      }
      else
        interactionMenu.interactionTarget = null;
    }

    function onMouseModeChanged() {
      interactionMenu.interactionTarget = null;
    }
  }
} // END INTERACTION MENU
