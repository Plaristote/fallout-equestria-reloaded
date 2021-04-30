import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Repeater {
  id: root
  property QtObject inventory
  property QtObject selectedObject
  property bool canEditArmor: true

  model: inventory.slotNames

  function updateSlots() {
    model = [];
    model = inventory.slotNames;
  }

  delegate: Pane {
    property string slotName: inventory.slotNames[index]
    property QtObject equippedItem: inventory.getEquippedItem(slotName)
    background: UiStyle.TerminalPane {}
    Layout.preferredHeight: 125
    Layout.preferredWidth: 125
    Layout.alignment: Qt.AlignHCenter
    clip: true

    ColumnLayout {
      anchors.fill: parent

      Text {
        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
        text: slotName
        font.family: application.titleFontName
        color: "yellow"
      }

      ItemIcon {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        model:  equippedItem
        Layout.maximumHeight: 50
        Layout.maximumWidth: 125
        visible: equippedItem.icon !== "any.png"
      }

      Row {
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter

        TerminalButton {
          visible: equippedItem !== null && equippedItem.isVirtual !== true
          text: "-"
          height: 20
          width: 70 / 2
          onClicked: inventory.unequipItem(slotName)
        }

        TerminalButton {
          visible: root.selectedObject !== null && inventory.canEquipItem(root.selectedObject, slotName)
          text: "+"
          height: 20
          width: 70 / 2
          onClicked: inventory.equipItem(selectedObject, slotName);
        }
      }
    }
  } // END Slot
} // END Slots
