import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  id: levelHud
  property QtObject gameController;
  property QtObject levelController;

  height: 135
  width: Math.min(parent.width, 1025)
  background: UiStyle.PlayPanel {}

  signal openMenu()
  signal openInventory()

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
        onClicked: openInventory()
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
        onClicked: openMenu()
      }
    }
  }
}
