import QtQuick 2.15

Rectangle {
  property bool isActive: gameManager.currentGame.fastPassTime
  property real delay: 525
  anchors.fill: parent
  opacity: isActive ? 1 : 0
  color: "black"
  Behavior on opacity { NumberAnimation { duration: delay } }

  Loader {
    sourceComponent: isActive ? clockWidget : null
    anchors.centerIn: parent
  }

  Component {
    id: clockWidget
    ClockWidget {
    }
  }
}
