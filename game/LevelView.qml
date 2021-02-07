import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Item {
  property QtObject gameController;
  property QtObject levelController;
  anchors.fill: parent

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
      anchors.left: parent.left
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      width: 250
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
  }
}
