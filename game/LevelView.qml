import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "./hud" as Hud
import "./level"

LevelDisplay {
  id: root
  property bool hasOverlay: interactionMenu.visible || inventoryViewContainer.visible || itemPickerContainer.visible || skilldex.visible || countdownDialog.visible || mainMenu.visible
  enabled: !gameController.fastPassTime

  function openMenu() {
    levelController.paused = !mainMenu.visible;
    if (interactionMenu.visible)
      interactionMenu.interactionTarget = null;
    mainMenu.visible = !mainMenu.visible;
  }

  onHasOverlayChanged: if (application.currentView === root) { levelController.paused = hasOverlay }
  onPickedObject: levelController.objectClicked(dynamicObject)
  onPickedTile:   levelController.tileClicked(tileX, tileY)

  Hud.Actions {
    id: actions
    level: root.levelController
    enabled: application.currentView === root
    onMenuTriggered:           root.openMenu()
    onPreviousTargetTriggered: levelController.centerCursorOn(levelController.targetList.previousTarget())
    onNextTargetTriggered:     levelController.centerCursorOn(levelController.targetList.nextTarget())
    onInventoryTriggered:      inventoryViewContainer.visible = true
    onSkilldexTriggered:       skilldex.visible = !skilldex.visible
    onDebugModeTriggered:      debugConsole.visible = !debugConsole.visible
    onBackTriggered: {
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
      else if (itemPickerContainer.visible)
        itemPicker.closed();
      else if (levelController.combat && levelController.isPlayerTurn)
        levelController.passTurn(levelController.player);
      else
        root.openMenu()
    }
  }

  Loader {
    anchors {
      top: parent.top; topMargin: 50
      bottom: levelHud.top; left: parent.left
    }
    visible: root.levelController.tutorial
    sourceComponent: visible ? tutorialPane : null
    width:  350
  }

  Item {
    id: levelCursor
    property real mouseX: levelMouseArea.mouseX
    property real mouseY: levelMouseArea.mouseY

    UsageSuccessHint {
      levelController: root.levelController
      target: levelMouseArea.hoveredObject
      targetTile: renderTarget.hoverTile
    }
  }

  DynamicObjectMetrics {
    levelController: root.levelController
    anchors.left: parent.left
    anchors.top: parent.top
  }

  LevelTextBubbles {
    origin: renderTarget.origin
  }

  Hud.InteractionMenu {
    id: interactionMenu
    levelController: root.levelController
    mouseArea: levelMouseArea
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

    onOpenMenu: actions.openMenu.trigger()
    onOpenPipboy: actions.openPipboy.trigger()
    onOpenInventory: actions.openInventory.trigger()
    onOpenCharacterSheet: actions.openCharacterSheet.trigger()
    onOpenSkilldex: {
      skilldex.target = null;
      actions.openSkilldex.trigger();
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
      actions.openSkilldex.trigger();
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
        levelController.useItemOn(itemPicker.selectedObject, itemPicker.target, itemPicker.selectedObject.defaultUseMode);
        target = selectedObject = null;
      }
    }
  }

  Hud.PlayerInventory {
    id: inventoryViewContainer
    anchors.fill: parent
    visible: false
    inventoryHeight: height - levelHud.height - 100
  }

  Hud.CountdownDialog {
    id: countdownDialog
    visible: false
  }

  FastTimePassView {}

  FpsWidget {
    anchors.top:   parent.top
    anchors.right: parent.right
    visible: levelController.debugMode
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

  Hud.Menu {
    id: mainMenu
    anchors.centerIn: parent
    visible: false
  }

  Component {
    id: tutorialPane
    Hud.TutorialPane {
      anchors.fill: parent
      controller: root.levelController.tutorial
    }
  }
}
