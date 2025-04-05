import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/ui"
import "qrc:/assets/ui" as UiStyle

Pane {
  property int itemIconWidth: 72
  property int itemIconHeight: 72
  property QtObject inventory
  property QtObject selectedObject
  property alias typeFilter: categoryFilter.currentText
  property var items: []
  property var dragZone
  id: root
  background: UiStyle.TerminalPane {}

  signal itemSelected(QtObject selectedItem)
  signal itemActivated(QtObject activatedItem)
  signal itemDropped(QtObject inventoryItem)

  function updateItemList() {
    if (typeFilter !== "") {
      const list = [];

      for (var i = 0 ; i < inventory.items.length ; ++i) {
        if (inventory.items[i].category === typeFilter)
          list.push(inventory.items[i]);
      }
      items = list;
    }
    else
      items = inventory.items;
  }

  onTypeFilterChanged: updateItemList()
  onInventoryChanged:  updateItemList()

  Connections {
    target: inventory
    function onItemsChanged() { updateItemList(); }
  }

  TerminalComboBox {
    id: categoryFilter
    anchors { left: parent.left; right: parent.right; top: parent.top }
    model: inventory ? inventory.categoryList : []
  }

  CustomFlickable {
    clip: true
    contentHeight: characterInventoryItemsView.height
    anchors.fill: parent
    anchors.topMargin: categoryFilter.height + 5
    anchors.bottomMargin: 15

    Grid {
      id: characterInventoryItemsView
      width: parent.width
      columns: parent.width / root.itemIconWidth
      spacing: 5

      Repeater {
        model: items.length
        delegate: InventoryItemHandle {
          inventoryItem: items[index]
          itemCount: inventoryItem.quantity
          selected: selectedObject == inventoryItem
          itemIconWidth: root.itemIconWidth
          itemIconHeight: root.itemIconHeight
          dragZone: root.dragZone
          onClicked: root.itemSelected(inventoryItem)
          onDoubleClicked: root.itemActivated(inventoryItem)
        }
      }
    }
  }

  InventoryItemDropArea {
    anchors.fill: parent
    onItemDropped: root.itemDropped(inventoryItem)
  }
}
