pragma ComponentBehavior: Bound
import QtQuick 2.12
import "qrc:/assets/ui" as UiStyle
import "../../ui"

UiStyle.UnderlayView {
  id: root
  sourceComponent: inventoryView
  itemCentered: false
  property real inventoryHeight: inventoryView.height

  DropArea {
    anchors.fill: parent
    keys: ["InventoryItem"]
    function receiveInventoryItem(inventoryItem) {
      gameManager.currentGame.player.inventory.dropItem(inventoryItem)
      if (typeof soundManager != "undefined")
        soundManager.play("put-down");
    }
  }

  Component {
    id: inventoryView
    CharacterInventory {
      character: gameManager.currentGame.player
      anchors {
        top: parent.top
        topMargin: 50
        left: parent.left
        leftMargin: root.width / 8
      }
      width: root.width * 3/4
      height: root.height - 200
      onClosed: root.toggle()
      onHeightChanged: root.inventoryHeight = height;
    }
  }
}
