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
    mainMenu.visible = !mainMenu.visible;
  }

  Action {
    id: openMenuAction
    shortcut: Shortcut {
      sequence: "Esc"
      onActivated: openMenuAction.trigger()
    }
    onTriggered: openMenu()
  }

  LevelCanvas {
    id: canvas
    levelController: parent.levelController
  }

  ScreenEdges {
    onMoveTop:    { canvas.translate(0, scrollSpeed); }
    onMoveLeft:   { canvas.translate(scrollSpeed, 0); }
    onMoveRight:  { canvas.translate(-scrollSpeed, 0); }
    onMoveBottom: { canvas.translate(0, -scrollSpeed); }
  }

  Pane {
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
            }
          }
        }
      }
    }

    Pane {
      anchors.left: terminalPane.right
      anchors.top: parent.top;
      anchors.bottom: parent.bottom
      anchors.topMargin: 1
      anchors.bottomMargin: 1
      anchors.leftMargin: 5
      width: 100
      background: UiStyle.Pane {}

      Column {
        anchors.fill: parent

        Button {
          text: "Q"
          height: 20
          width: parent.width
          onClicked: {
            console.log("Wtz");
            terminalPane.parent.parent.open
            root.openMenu();
          }
        }
      }
    }
  }

  Pane {
    id: mainMenu
    width: 280
    height: 400
    anchors.centerIn: parent
    visible: false
    background: UiStyle.Pane {}

    Column {
      anchors.fill: parent

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
