import QtQuick 2.15

Rectangle {
  property bool isActive: gameManager.currentGame.fastPassTime
  anchors.fill: parent
  opacity: isActive ? 1 : 0
  color: "black"
  Behavior on opacity { NumberAnimation { duration: 525 } }

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
