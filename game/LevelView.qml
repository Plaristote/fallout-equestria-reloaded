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

  Text {
    anchors { top: parent.top; right: parent.right }
    text: "Frame Rate: " + myframeRate
    font.pointSize: 20
    styleColor: "white"; style: Text.Outline
    property int myframeRate: 0
    Timer {
      interval: 1000; repeat: true; running: true
      onTriggered: {
        parent.myframeRate = canvas.controller.frameCount;
        canvas.controller.frameCount = 0;
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

  // BOTTOM PANE
  Pane {
    id: levelHud
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    height: 135
    width: Math.min(parent.width, 1025)
    background: UiStyle.PlayPanel {}

    Pane {
      id: terminalPane
      anchors.left: parent.left
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      width: 240
      background: UiStyle.TerminalPane {}

      Flickable {
        id: terminalFlickable
        anchors.fill: parent
        contentHeight: terminalContent.height
        clip: true

        ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

        Column {
          id: terminalContent
          width: parent.width - 10
          onHeightChanged: terminalFlickable.contentY = Math.max(0, terminalContent.height - terminalFlickable.height);

          Repeater {
            model: gameController.consoleMessages
            delegate: Text {
              wrapMode: Text.WordWrap
              width: terminalContent.width
              text: "> " + gameController.consoleMessages[index]
              color: "green"
              font.family: application.consoleFontName
              font.pointSize: 6
            }
          }
        }
      }
    }

    Pane {
      anchors.left: terminalPane.right
      anchors.top: parent.top;
      anchors.bottom: parent.bottom
      anchors.topMargin: 20
      anchors.bottomMargin: 1
      anchors.leftMargin: 5
      width: 80
      background: UiStyle.Pane {}

      Column {
        anchors.fill: parent
        //anchors.topMargin: 10
        spacing: 5

        Button {
          text: "INV"
          height: 20
          width: parent.width
          action: openInventoryAction
        }

        Button {
          text: "CHA"
          height: 20
          width: parent.width
          onClicked: {
            application.pushView("game/CharacterView.qml", {gameController: root.gameController})
          }
        }

        Button {
          text: "Q"
          height: 20
          width: parent.width
          onClicked: {
            terminalPane.parent.parent.open
            root.openMenu();
          }
        }
      }
    }
  }
  // END Bottom Pane

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

  // MainMenu
  Pane {
    id: mainMenu
    width: 280
    height: 250
    anchors.centerIn: parent
    visible: false
    background: UiStyle.Pane {}

    Column {
      anchors.fill: parent
      anchors.topMargin: 10

      MenuButton {
        text: "Save Game"
        onClicked: application.pushView("SaveGame.qml");
      }

      MenuButton {
        text: "Load Game"
        onClicked: application.pushView("LoadGame.qml")
      }

      MenuButton {
        text: "Exit"
        onClicked: {
          console.log("exit");
          application.popView();
          gameManager.endGame();
        }
      }

      MenuButton {
        text: "Cancel"
        action: openMenuAction
      }
    }
  }
}
