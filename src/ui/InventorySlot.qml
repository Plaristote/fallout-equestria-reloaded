import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Pane {
  id: root
  property string slotName: inventory.slotNames[index]
  property QtObject equippedItem: inventory.getEquippedItem(slotName)
  property QtObject selectedItem
  property var dragZone
  implicitHeight: 125
  implicitWidth: 125
  clip: true

  ColumnLayout {
    anchors.fill: parent

    Text {
      Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
      text: i18n.t(`item-slots.${slotName}`)
      font.family: application.titleFontName
      color: "yellow"
    }

    DraggableItemIcon {
      id: itemIcon
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      model:  equippedItem
      Layout.maximumHeight: 50
      Layout.maximumWidth: 115
      visible: equippedItem && equippedItem.icon !== "any.png"
      dragZone: root.dragZone
    }

    Row {
      Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter

      TerminalButton {
        visible: equippedItem != null && equippedItem.isVirtual !== true
        text: "✖"
        height: 20
        width: 25
        onClicked: inventory.unequipItem(slotName)
      }

      TerminalButton {
        visible: root.selectedItem != null && inventory.canEquipItem(root.selectedItem, slotName)
        text: "✓"
        height: 20
        width: 25
        font.bold: true
        onClicked: inventory.equipItem(root.selectedItem, slotName);
      }
    }
  }

  DropArea {
    anchors.fill: parent
    onDropped: {
      if (drop.source.dragType === "InventoryItem")
        receiveInventoryItem(drop.source.inventoryItem);
    }

    function receiveInventoryItem(inventoryItem) {
      if (inventory.canEquipItem(inventoryItem, slotName) && inventoryItem !== equippedItem)
        inventory.equipItem(inventoryItem, slotName);
    }
  }
} // END Slot
