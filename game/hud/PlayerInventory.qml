import QtQuick 2.12
import "../../ui"

Rectangle {
  id: root
  color: Qt.rgba(0, 0, 0, 0.5)
  property alias inventoryHeight: inventoryView.height

  MouseArea {
    anchors.fill: parent
  }

  CharacterInventory {
    id: inventoryView
    character: gameManager.currentGame.player
    anchors { top: parent.top; left: parent.left; right: parent.right }
    anchors.leftMargin:  parent.width / 8
    anchors.rightMargin: parent.width / 8
    anchors.bottomMargin: 50
    anchors.topMargin: 50
    height: parent.height - 200
    onClosed: root.visible = false
  }
}
