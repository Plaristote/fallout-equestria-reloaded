import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Pane {
  property string slotName: inventory.slotNames[index]
  property QtObject equippedItem: inventory.getEquippedItem(slotName)
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

    ItemIcon {
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      model:  equippedItem
      Layout.maximumHeight: 50
      Layout.maximumWidth: 115
      visible: equippedItem && equippedItem.icon !== "any.png"
    }

    Row {
      Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter

      TerminalButton {
        visible: equippedItem !== null && equippedItem.isVirtual !== true
        text: "✖"
        height: 20
        width: 25
        onClicked: inventory.unequipItem(slotName)
      }

      TerminalButton {
        visible: root.selectedObject !== null && inventory.canEquipItem(root.selectedObject, slotName)
        text: "✓"
        height: 20
        width: 25
        font.bold: true
        onClicked: inventory.equipItem(selectedObject, slotName);
      }
    }
  }

  DropArea {
    anchors.fill: parent
    onDropped: {
      console.log("Dropped on slot", drop.source.dragType);
      if (drop.source.dragType === "InventoryItem")
        receiveInventoryItem(drop.source.inventoryItem);
    }

    function receiveInventoryItem(inventoryItem) {
      if (inventory.canEquipItem(inventoryItem, slotName) )
        inventory.equipItem(inventoryItem, slotName);
    }
  }
} // END Slot
