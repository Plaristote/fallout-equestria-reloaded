import QtQuick 2.15

Loader {
  id: root
  sourceComponent: dropArea

  signal itemDropped(QtObject inventoryItem)

  Timer {
    interval: 500
    running: root.sourceComponent != dropArea
    onTriggered: root.sourceComponent = dropArea
  }

  Component {
    id: dropArea
    DropArea {
      keys: ["InventoryItem"]
      function receiveInventoryItem(inventoryItem) {
        root.itemDropped(inventoryItem);
        root.sourceComponent = null;
      }
    }
  }
}
