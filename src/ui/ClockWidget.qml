import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../assets/ui" as UiStyle

Item {
  property QtObject timeManager: gameManager.currentGame.timeManager
  id: clockViewContainer
  implicitHeight: clockView.height + 15
  implicitWidth:  Math.max(clockView.width, timeDisplay.width)

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

  TimeDisplay {
    id: timeDisplay
    timeManager: clockViewContainer.timeManager
    height: clockView.height / 2 + 15
    anchors.horizontalCenter: clockView.horizontalCenter
    anchors.bottom: clockView.bottom
    anchors.bottomMargin: -10
  }
}
