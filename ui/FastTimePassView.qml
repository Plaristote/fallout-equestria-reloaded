import QtQuick 2.15

Rectangle {
  anchors.fill: parent
  opacity: gameManager.currentGame.fastPassTime ? 1 : 0
  color: "black"
  Behavior on opacity { NumberAnimation { duration: 525 } }
}
