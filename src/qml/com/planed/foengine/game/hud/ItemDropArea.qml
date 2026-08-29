import QtQuick 2.15

DropArea {
  property QtObject pendingItem: null
  property var hoverTile

  anchors.fill: parent
  keys: ["InventoryItem"]

  Timer {
    id: dropTimer
    interval: 500
    running: pendingItem != null
    onTriggered: dropItem()
  }

  function receiveInventoryItem(inventoryItem) {
    pendingItem = inventoryItem;
  }

  function dropItem() {
    const actionQueue = levelController.player.actionQueue;
    const inventory = levelController.player.inventory;

    console.log("Dropped item", pendingItem.itemType, "at", hoverTile)
    actionQueue.reset();
    actionQueue.pushMovement(...hoverTile);
    actionQueue.pushScript(inventory.dropItem.bind(inventory, pendingItem));
    actionQueue.start();
    pendingItem = null;
  }
}
