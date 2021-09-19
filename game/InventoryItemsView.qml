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
  id: root
  background: UiStyle.TerminalPane {}

  signal itemSelected(QtObject selectedItem)

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
    model: inventory.categoryList
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
        delegate: Rectangle {
          property QtObject inventoryItem: items[index]
          property int itemCount: inventoryItem.quantity
          property bool selected: selectedObject == inventoryItem

          width: Math.max(root.itemIconWidth, root.itemIconHeight)
          height: width
          border.width: root.selectedObject === inventoryItem ? 1 : 0
          border.color: "green"
          radius: 5
          color: "transparent"
          ItemIcon {
            anchors.centerIn: parent
            model: inventoryItem
            width: root.itemIconWidth
            height: root.itemIconHeight
          }
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            text: itemCount > 1 ? `x${itemCount}` : ""
            font.family: application.consoleFontName
            font.pointSize: 8
            style: Text.Raised
            styleColor: "black"
            color: "white"
            width: root.itemIconWidth
            wrapMode: Text.WrapAnywhere
          }
          MouseArea {
            id: itemMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.itemSelected(inventoryItem);
          }
        }
      }
    }
  }
}
