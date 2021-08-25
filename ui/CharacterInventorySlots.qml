import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  id: root
  property QtObject inventory
  property QtObject selectedObject
  property bool canEditArmor: true
  property var layout

  function updateSlots() {
    repeater.model = [];
    repeater.model = inventory.slotNames;
  }

  background: UiStyle.TerminalPane {}

  Image {
    anchors.centerIn: parent
    source: `qrc:/assets/ui/equipment/${layout.source}`
    Repeater {
      id: repeater
      model: inventory.slotNames
      delegate: InventorySlot {
        slotName: inventory.slotNames[index]
        equippedItem: inventory.getEquippedItem(slotName)
        background: Rectangle { color: "transparent" }
        x: layout[slotName].x
        y: layout[slotName].y
      } // END Slot
    } // END Slots
  }
}
