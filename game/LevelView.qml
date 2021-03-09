import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Item {
  id: root
  property QtObject gameController;
  property QtObject levelController;
  anchors.fill: parent

  function openMenu() {
    levelController.paused = !mainMenu.visible;
    if (interactionMenu.visible)
      interactionMenu.interactionTarget = null;
    mainMenu.visible = !mainMenu.visible;
  }

  Shortcut {
    sequence: "Esc"
    onActivated: {
      if (interactionMenu.visible)
        interactionMenu.interactionTarget = null;
      else if (inventoryView.visible)
        inventoryView.visible = false;
      else
        openMenuAction.trigger()
    }
  }

  Action {
    id: openMenuAction
    onTriggered: openMenu()
  }

  Action {
    id: openInventoryAction
    shortcut: Shortcut {
      sequence: "i"
      onActivated: openInventoryAction.trigger()
    }
    onTriggered: {
      inventoryView.visible = true;
    }
  }

  Action {
    id: openSkilldexAction
    shortcut: Shortcut {
      sequence: "s"
      onActivated: openSkilldexAction.trigger()
    }
    onTriggered: skilldex.visible = !skilldex.visible
  }

  LevelCanvas {
    id: canvas
    levelController: parent.levelController
  }

  ScreenEdges {
    enabled: !parent.levelController.paused
    onMoveTop:    { canvas.translate(0, scrollSpeed); }
    onMoveLeft:   { canvas.translate(scrollSpeed, 0); }
    onMoveRight:  { canvas.translate(-scrollSpeed, 0); }
    onMoveBottom: { canvas.translate(0, -scrollSpeed); }
  }

  LevelFrameRate {
    target: canvas
    anchors { top: parent.top; right: parent.right }
  }

  // TEXT BUBBLES
  Repeater {
    model: levelController.textBubbles
    delegate: Rectangle {
      property QtObject textBubble: levelController.textBubbles[index]
      color: Qt.rgba(0, 0, 0, 0.5)
      border.color: "white"
      border.width: 1
      radius: 10
      width: 150
      height: textBubbleContentView.height
      opacity: textBubble.opacity

      x: textBubble.position.x + canvas.origin.x + (72 / 2) - width / 2
      y: textBubble.position.y + canvas.origin.y - height

      Text {
        id: textBubbleContentView
        padding: 5
        width: parent.width
        wrapMode: Text.WordWrap
        text: textBubble.content
        color: textBubble.color
      }
    }
  }

  // INTERACTION MENU
  Item {
    property QtObject interactionTarget
    property var interactionTypes: []
    property point interactionPosition
    id: interactionMenu
    visible: interactionTarget && interactionTypes.length > 0

    x: interactionPosition.x + canvas.origin.x
    y: interactionPosition.y + canvas.origin.y

    onVisibleChanged: {
      levelController.paused = visible;
    }

    Column {
      Repeater {
        model: interactionMenu.interactionTypes
        delegate: Button {
          id: button
          hoverEnabled: true

          background: Image {
            property string bgType: parent.down ? 'pressed' : (parent.hovered ? 'active' : 'normal')
            source: "qrc:/assets/ui/interactions/" + interactionMenu.interactionTypes[index] + '-' + bgType + '.png'
          }

          onClicked:{
            levelController.interactOrderReceived(interactionMenu.interactionTarget, interactionMenu.interactionTypes[index]);
            interactionMenu.interactionTarget = null;
          }
        }
      }
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
        interactionMenu.interactionPosition = interactionTarget.getSpritePosition();
      }
      else
        interactionMenu.interactionTarget = null;
    }

    function onStartDialog(dialogController) {
      console.log("ztarting dialog controller", dialogController);
      console.log("text iz ", dialogController.text);
      application.pushView("game/Dialog.qml", {controller: dialogController});
      levelController.paused = true;
    }

    function onStartLooting(lootingController) {
      console.log("ztarting looting controller", lootingController);
      application.pushView("game/Looting.qml", {controller: lootingController});
      levelController.paused = true;
    }
  }

  LevelHud {
    id: levelHud
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    gameController:  root.gameController
    levelController: root.levelController

    onOpenMenu: openMenuAction.trigger()
    onOpenInventory: openInventoryAction.trigger()
    onOpenSkilldex: openSkilldexAction.trigger()
  }

  Skilldex {
    id: skilldex
    anchors.bottom: levelHud.top
    anchors.right: parent.right
    width: 240
    visible: false
    character: levelController.player
    onPickedSkill: {
      openSkilldexAction.trigger();
      levelController.useSkill(skillName);
    }
  }

  CharacterInventory {
    id: inventoryView
    character: levelController.player
    anchors { top: parent.top; left: parent.left; bottom: levelHud.top; right: parent.right }
    anchors.leftMargin:  parent.width > 1200 ? parent.width / 4 : parent.width / 8
    anchors.rightMargin: parent.width > 1200 ? parent.width / 4 : parent.width / 8
    anchors.bottomMargin: 50
    anchors.topMargin: 50
    visible: false
    onVisibleChanged: levelController.paused = visible
    onClosed: visible = false
  }

  LevelMenu {
    id: mainMenu
    anchors.centerIn: parent
    visible: false
    onVisibleChanged: levelController.paused = visible
  }
}
