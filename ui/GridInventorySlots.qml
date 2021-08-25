import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "../assets/ui" as UiStyle

GridLayout {
  id: root
  property QtObject inventory
  property QtObject selectedObject
  property bool canEditArmor: true

  function updateSlots() {
    repeater.model = [];
    repeater.model = inventory.slotNames;
  }

  columns: Math.min(2, parent.width / 125)

  Repeater {
    id: repeater
    model: inventory.slotNames

    delegate: InventorySlot {
      slotName: inventory.slotNames[index]
      equippedItem: inventory.getEquippedItem(slotName)
      background: UiStyle.TerminalPane {}
      Layout.alignment: Qt.AlignHCenter
    } // END Slot
  } // END Slots
}
