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
  property bool hscrollbar: view.width > scrollable.width
  property bool vscrollbar: view.height > scrollable.height

  function updateSlots() {
    repeater.model = [];
    repeater.model = inventory.slotNames;
  }

  background: UiStyle.TerminalPane {}

  CustomFlickable {
    id: scrollable
    anchors.fill: parent
    clip: true
    contentHeight: Math.max(view.height + (vscrollbar || hscrollbar ? 50 : 0), root.height)
    contentWidth:  Math.max(view.width  + (hscrollbar || vscrollbar ? 50 : 0), root.width)

    ScrollBar.vertical:   UiStyle.TerminalScrollbar {
      orientation: Qt.Vertical
      visible: vscrollbar || hscrollbar
    }
    ScrollBar.horizontal: UiStyle.TerminalScrollbar {
      orientation: Qt.Horizontal
      visible: hscrollbar || vscrollbar
    }

    Image {
      id: view
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
}
