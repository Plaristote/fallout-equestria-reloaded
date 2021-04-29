import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "./hud" as Hud

Item {
  id: root
  property QtObject gameController
  property QtObject levelController
  property bool hasOverlay: interactionMenu.visible || inventoryViewContainer.visible || itemPickerContainer.visible || skilldex.visible || countdownDialog.visible || mainMenu.visible
  anchors.fill: parent

  function openMenu() {
    levelController.paused = !mainMenu.visible;
    if (interactionMenu.visible)
      interactionMenu.interactionTarget = null;
    mainMenu.visible = !mainMenu.visible;
  }

  onHasOverlayChanged: levelController.paused = hasOverlay

  Shortcut {
    sequence: "Esc"
    onActivated: {
      if  (mainMenu.visible)
        mainMenu.visible = false;
      else if (interactionMenu.visible)
        interactionMenu.interactionTarget = null;
      else if (countdownDialog.visible)
        countdownDialog.visible = false;
      else if (inventoryViewContainer.visible)
        inventoryViewContainer.visible = false;
      else if (skilldex.visible)
        skilldex.visible = false;
      else
        openMenuAction.trigger()
    }
  }

  Shortcut {
    sequence: "F2"
    onActivated: debugConsole.visible = !debugConsole.visible
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
      inventoryViewContainer.visible = true;
    }
  }

  Action {
    id: openSkilldexAction
    shortcut: Shortcut {
      sequence: "s"
      onActivated: openSkilldexAction.trigger()
    }
    onTriggered: {
      skilldex.visible = !skilldex.visible;
    }
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

  Hud.InteractionMenu {
    id: interactionMenu
    levelController: root.levelController
    levelCanvas: canvas
    bottomLimit: root.height - levelHud.height
  }

  Connections {
    target: levelController

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

    function onCountdownRequired(item) {
      countdownDialog.item = item;
      countdownDialog.visible = true;
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
    onOpenSkilldex: {
      skilldex.target = null;
      openSkilldexAction.trigger();
    }
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
      if (skilldex.target)
        levelController.useSkillOn(levelController.player, skilldex.target, skillName);
      else
        levelController.useSkill(skillName);
    }
  }

  Rectangle {
    id: itemPickerContainer
    color: Qt.rgba(0, 0, 0, 0.5)
    anchors.fill: parent
    visible: false

    MouseArea {
      anchors.fill: parent
    }

    Hud.ItemPicker {
      id: itemPicker
      anchors { top: parent.top; left: parent.left; right: parent.right }
      anchors.leftMargin:  parent.width > 1200 ? parent.width / 4 : parent.width / 8
      anchors.rightMargin: parent.width > 1200 ? parent.width / 4 : parent.width / 8
      anchors.bottomMargin: 50
      anchors.topMargin: 50
      height: parent.height - levelHud.height
      inventory: levelController.player.inventory
      onClosed: {
        itemPickerContainer.visible = false;
        target = selectedObject = null;
      }
      onAccepted: {
        itemPickerContainer.visible = false;
        levelController.useItemOn(itemPicker.selectedObject, itemPicker.target);
        target = selectedObject = null;
      }
    }
  }

  Rectangle {
    id: inventoryViewContainer
    color: Qt.rgba(0, 0, 0, 0.5)
    anchors.fill: parent
    visible: false

    MouseArea {
      anchors.fill: parent
    }

    CharacterInventory {
      id: inventoryView
      character: levelController.player
      anchors { top: parent.top; left: parent.left; right: parent.right }
      anchors.leftMargin:  parent.width > 1200 ? parent.width / 4 : parent.width / 8
      anchors.rightMargin: parent.width > 1200 ? parent.width / 4 : parent.width / 8
      anchors.bottomMargin: 50
      anchors.topMargin: 50
      height: parent.height - levelHud.height
      onClosed: inventoryViewContainer.visible = false
    }
  }

  Hud.CountdownDialog {
    id: countdownDialog
    visible: false
  }

  Hud.DebugConsole {
    id: debugConsole
    visible: false
    gameController: root.gameController
    anchors {
      left: parent.left; right: parent.right
      top: parent.top
    }
  }

  LevelMenu {
    id: mainMenu
    anchors.centerIn: parent
    visible: false
  }
}
