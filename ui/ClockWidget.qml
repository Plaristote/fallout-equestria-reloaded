import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../assets/ui" as UiStyle

Item {
  property QtObject timeManager: gameManager.currentGame.timeManager
  id: clockViewContainer
  implicitHeight: clockView.height
  implicitWidth:  clockView.width

  Image {
    id: clockView
    source: "qrc:/assets/ui/clock.png"
    width: 150
    height: 150
    anchors.horizontalCenter: parent.horizontalCenter
    rotation: (timeManager.hour + (timeManager.minute / 60)) * -15 -220
  }

  Image {
    source: "qrc:/assets/ui/clock-overlay.png"
    anchors.fill: clockView
  }

  Pane {
    background: UiStyle.TerminalPane {}
    height: clockView.height / 2 + 10
    anchors.left: clockView.left
    anchors.right: clockView.right
    anchors.bottom: clockView.bottom

    Text {
      id: dateText
      text: renderDateText()
      color: "white"
      anchors.centerIn: parent

      function renderDateText() {
        return timeManager.year + "/"
            + ('0' + timeManager.month).slice(-2) + "/"
            + ('0' + timeManager.day).slice(-2) + " "
            + ('0' + timeManager.hour).slice(-2) + ":"
            + ('0' + timeManager.minute).slice(-2);
      }
    }
  }
}
